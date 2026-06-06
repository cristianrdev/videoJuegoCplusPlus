#include "Game.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <initializer_list>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {
constexpr auto WindowWidth = 1920u;
constexpr auto WindowHeight = 1080u;

bool intersects(sf::FloatRect left, sf::FloatRect right) {
    return left.position.x < right.position.x + right.size.x &&
        left.position.x + left.size.x > right.position.x &&
        left.position.y < right.position.y + right.size.y &&
        left.position.y + left.size.y > right.position.y;
}

int firstConnectedJoystick() {
    for (auto joystick = 0u; joystick < sf::Joystick::Count; ++joystick) {
        if (sf::Joystick::isConnected(joystick)) {
            return static_cast<int>(joystick);
        }
    }

    return -1;
}

bool joystickButtonPressedAny(std::initializer_list<unsigned int> buttons) {
    const auto joystick = firstConnectedJoystick();
    if (joystick < 0) {
        return false;
    }

    const auto id = static_cast<unsigned int>(joystick);
    for (const auto button : buttons) {
        if (button < sf::Joystick::getButtonCount(id) &&
            sf::Joystick::isButtonPressed(id, button)) {
            return true;
        }
    }

    return false;
}

bool joystickFirePressed() {
    return joystickButtonPressedAny({
        0,  // Xbox A / PlayStation Cross / many generic primary buttons
        1,  // Xbox B / PlayStation Circle
        2,  // Xbox X / PlayStation Square
        3,  // Xbox Y / PlayStation Triangle
        4,  // LB / L1
        5   // RB / R1
    });
}

bool joystickPauseButton(unsigned int button) {
    return button == 7 ||  // Xbox Menu / common Start mapping
        button == 8 ||     // common generic Start mapping
        button == 9;       // PlayStation Options on many mappings
}
}

Game::Game()
    : window_(sf::VideoMode::getDesktopMode(), "Shooter vertical", sf::State::Fullscreen)
    , logicalTarget_({LogicalWidth, LogicalHeight})
    , presentationSprite_(logicalTarget_.getTexture())
    , debugText_(debugFont_)
    , starfield_({static_cast<float>(LogicalWidth), static_cast<float>(LogicalHeight)})
    , assets_("assets") {
    window_.setVerticalSyncEnabled(true);
    logicalTarget_.setSmooth(false);

    if (!debugFont_.openFromFile("C:/Windows/Fonts/consola.ttf")) {
        throw std::runtime_error("No se pudo cargar fuente debug: C:/Windows/Fonts/consola.ttf");
    }

    debugText_.setCharacterSize(16);
    debugText_.setFillColor(sf::Color(190, 220, 230));

    playerConfigSystem_.loadFromFile("config/player.json");
    const auto& playerConfig = playerConfigSystem_.config();
    assets_.loadTexture("player_ship_sheet", playerConfig.shipTexture);
    assets_.loadTexture("player_thruster_flame", playerConfig.thrusterTexture);
    laserNormalTexture_ = &assets_.loadTexture("player_laser_normal", playerConfig.laserTexture);
    muzzleFlashTexture_ = &assets_.loadTexture("player_laser_muzzle_flash", playerConfig.muzzleFlashTexture);
    enemyConfigSystem_.loadFromFile("config/enemies.json");
    for (const auto& enemyId : enemyConfigSystem_.enemyIds()) {
        assets_.loadTexture(enemyId, enemyConfigSystem_.texturePathFor(enemyId));
    }
    explosionDroneTexture_ = &assets_.loadTexture("explosion_enemy_drone", "textures/effects/explosion_enemy_drone.png");
    explosionTurretPodTexture_ = &assets_.loadTexture("explosion_enemy_turret_pod", "textures/effects/explosion_enemy_turret_pod.png");
    explosionInterceptorTexture_ = &assets_.loadTexture("explosion_enemy_interceptor", "textures/effects/explosion_enemy_interceptor.png");
    projectileConfigSystem_.loadFromFile("config/projectiles.json");
    for (const auto& projectileId : projectileConfigSystem_.projectileIds()) {
        assets_.loadTexture(projectileId, projectileConfigSystem_.texturePathFor(projectileId));
    }
    floatingRedRocksTexture_ = &assets_.loadTexture("floating_red_rocks_tileset", "textures/background/floating_red_rocks_tileset.png");
    bulletPatternSystem_.loadFromFile("config/bullet_patterns.json");
    movementPatternSystem_.loadFromFile("config/movement_patterns.json");
    stageDirector_.loadFromFile("config/stage_01_enemies.json");
    backgroundElementDirector_.loadFromFile("config/stage_01_background_elements.json");

    player_ = std::make_unique<Player>(
        assets_,
        sf::Vector2f{static_cast<float>(LogicalWidth), static_cast<float>(LogicalHeight)},
        playerConfigSystem_.config()
    );

    updatePresentationSprite();
}

void Game::run() {
    while (window_.isOpen()) {
        const auto deltaTime = clock_.restart();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    while (const auto event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_.close();
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                window_.close();
            } else if (keyPressed->code == sf::Keyboard::Key::P) {
                togglePause();
            } else if (keyPressed->code == sf::Keyboard::Key::Num1) {
                presentationScaleMode_ = PresentationScaleMode::Native;
                updatePresentationSprite();
            } else if (keyPressed->code == sf::Keyboard::Key::Num2) {
                presentationScaleMode_ = PresentationScaleMode::IntegerFit;
                updatePresentationSprite();
            } else if (keyPressed->code == sf::Keyboard::Key::Space ||
                       keyPressed->code == sf::Keyboard::Key::Z) {
                fireLaserNormal();
            }
        }

        if (const auto* buttonPressed = event->getIf<sf::Event::JoystickButtonPressed>()) {
            if (joystickPauseButton(buttonPressed->button)) {
                togglePause();
            }
        }
    }
}

void Game::togglePause() {
    paused_ = !paused_;
    if (paused_) {
        muzzleFlashTime_ = sf::Time::Zero;
    }
}

void Game::fireLaserNormal() {
    if (paused_ || fireCooldown_ > sf::Time::Zero || !player_) {
        return;
    }

    const auto& config = playerConfigSystem_.config();
    playerLasers_.emplace_back(
        player_->laserSpawnPosition(),
        *laserNormalTexture_,
        config.laserSpeed,
        config.laserDamage
    );
    fireCooldown_ = sf::seconds(config.fireCooldownSeconds);
    muzzleFlashTime_ = sf::seconds(config.muzzleFlashSeconds);
}

void Game::spawnEnemy(const StageDirector::SpawnEvent& spawn) {
    const auto& texture = assets_.getTexture(spawn.enemyId);

    enemies_.emplace_back(
        sf::Vector2f{spawn.x, spawn.y},
        texture,
        spawn.enemyId,
        spawn.patternId,
        spawn.movementId,
        enemyConfigSystem_.healthFor(spawn.enemyId)
    );
}

void Game::spawnBackgroundElement(const BackgroundElementDirector::SpawnEvent& spawn) {
    if (spawn.tilesetId != "floating_red_rocks") {
        throw std::runtime_error("Tileset de fondo no configurado: " + spawn.tilesetId);
    }

    backgroundElements_.emplace_back(
        sf::Vector2f{spawn.x, spawn.y},
        spawn.speedY,
        *floatingRedRocksTexture_,
        spawn.tileIndex,
        sf::Vector2i{100, 100}
    );
}

void Game::spawnExplosion(const Enemy& enemy) {
    auto texture = explosionDroneTexture_;

    if (enemy.enemyId() == "enemy_turret_pod") {
        texture = explosionTurretPodTexture_;
    } else if (enemy.enemyId() == "enemy_interceptor") {
        texture = explosionInterceptorTexture_;
    }

    explosions_.emplace_back(enemy.position(), *texture);
}

void Game::update(sf::Time deltaTime) {
    const auto frameSeconds = deltaTime.asSeconds();
    if (frameSeconds > 0.f) {
        const auto instantFps = 1.f / frameSeconds;
        smoothedFps_ = smoothedFps_ == 0.f ? instantFps : smoothedFps_ * 0.90f + instantFps * 0.10f;
    }

    if (paused_) {
        return;
    }

    stageClock_ += deltaTime;

    if (fireCooldown_ > sf::Time::Zero) {
        fireCooldown_ -= deltaTime;
        if (fireCooldown_ < sf::Time::Zero) {
            fireCooldown_ = sf::Time::Zero;
        }
    }

    if (muzzleFlashTime_ > sf::Time::Zero) {
        muzzleFlashTime_ -= deltaTime;
        if (muzzleFlashTime_ < sf::Time::Zero) {
            muzzleFlashTime_ = sf::Time::Zero;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) ||
        joystickFirePressed()) {
        fireLaserNormal();
    }

    for (const auto& spawn : stageDirector_.update(deltaTime)) {
        spawnEnemy(spawn);
    }

    for (const auto& spawn : backgroundElementDirector_.update(deltaTime)) {
        spawnBackgroundElement(spawn);
    }

    starfield_.update(deltaTime);
    for (auto& element : backgroundElements_) {
        element.update(deltaTime);
    }

    player_->update(deltaTime);

    for (auto& laser : playerLasers_) {
        laser.update(deltaTime);
    }

    for (auto& enemy : enemies_) {
        enemy.update(deltaTime);
        enemy.setPosition(
            movementPatternSystem_.positionFor(
                enemy.movementId(),
                enemy.startPosition(),
                enemy.elapsed()
            )
        );
    }

    updateEnemyShooting();

    for (auto& bullet : enemyBullets_) {
        bullet.update(deltaTime);
    }

    for (auto& laser : enemyLasers_) {
        laser.update(deltaTime);
    }

    for (auto& explosion : explosions_) {
        explosion.update(deltaTime);
    }

    playerLasers_.erase(
        std::remove_if(
            playerLasers_.begin(),
            playerLasers_.end(),
            [](const LaserNormal& laser) {
                return !laser.isAlive();
            }
        ),
        playerLasers_.end()
    );

    enemyBullets_.erase(
        std::remove_if(
            enemyBullets_.begin(),
            enemyBullets_.end(),
            [](const EnemyBullet& bullet) {
                return !bullet.isAlive({static_cast<float>(LogicalWidth), static_cast<float>(LogicalHeight)});
            }
        ),
        enemyBullets_.end()
    );

    enemyLasers_.erase(
        std::remove_if(
            enemyLasers_.begin(),
            enemyLasers_.end(),
            [](const EnemyLaser& laser) {
                return !laser.isAlive();
            }
        ),
        enemyLasers_.end()
    );

    backgroundElements_.erase(
        std::remove_if(
            backgroundElements_.begin(),
            backgroundElements_.end(),
            [](const BackgroundElement& element) {
                return !element.isAlive(static_cast<float>(LogicalHeight));
            }
        ),
        backgroundElements_.end()
    );

    updateCollisions();

    enemies_.erase(
        std::remove_if(
            enemies_.begin(),
            enemies_.end(),
            [](const Enemy& enemy) {
                return !enemy.isAlive();
            }
        ),
        enemies_.end()
    );

    explosions_.erase(
        std::remove_if(
            explosions_.begin(),
            explosions_.end(),
            [](const Explosion& explosion) {
                return explosion.isFinished();
            }
        ),
        explosions_.end()
    );
}

void Game::updateEnemyShooting() {
    if (!player_) {
        return;
    }

    for (auto& enemy : enemies_) {
        if (!enemy.shouldFire()) {
            continue;
        }

        if (bulletPatternSystem_.patternType(enemy.patternId()) == "continuous_laser") {
            const auto origin = enemy.bulletSpawnPosition();
            const auto length = std::max(0.f, static_cast<float>(LogicalHeight) - origin.y);
            const auto duration = sf::seconds(bulletPatternSystem_.laserDuration(enemy.patternId()));
            const auto* laserTexture = laserTextureForPattern(enemy.patternId());
            if (length > 0.f && laserTexture) {
                enemyLasers_.emplace_back(
                    origin,
                    length,
                    duration,
                    *laserTexture
                );
                enemy.startFiringVisual(duration);
            }
            enemy.resetFireTimer(bulletPatternSystem_.fireInterval(enemy.patternId()));
            continue;
        }

        auto bullets = bulletPatternSystem_.spawn(
            enemy.patternId(),
            enemy.bulletSpawnPosition(),
            player_->position(),
            bulletTextureForPattern(enemy.patternId())
        );
        enemyBullets_.insert(enemyBullets_.end(), bullets.begin(), bullets.end());
        enemy.resetFireTimer(bulletPatternSystem_.fireInterval(enemy.patternId()));
    }
}

void Game::updateCollisions() {
    for (auto laserIt = playerLasers_.begin(); laserIt != playerLasers_.end();) {
        auto hitEnemy = false;

        for (auto& enemy : enemies_) {
            if (!enemy.isAlive()) {
                continue;
            }

            if (intersects(laserIt->hitbox(), enemy.hitbox())) {
                enemy.takeDamage(laserIt->damage());
                if (!enemy.isAlive()) {
                    spawnExplosion(enemy);
                }
                hitEnemy = true;
                break;
            }
        }

        if (hitEnemy) {
            laserIt = playerLasers_.erase(laserIt);
        } else {
            ++laserIt;
        }
    }

    if (!player_) {
        return;
    }

    for (auto bulletIt = enemyBullets_.begin(); bulletIt != enemyBullets_.end();) {
        if (intersects(bulletIt->hitbox(), player_->hitbox())) {
            player_->takeDamage(1);
            bulletIt = enemyBullets_.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }

    for (auto& laser : enemyLasers_) {
        if (laser.canHitPlayer() && intersects(laser.hitbox(), player_->hitbox())) {
            player_->takeDamage(1);
            laser.markPlayerHit();
        }
    }
}

void Game::render() {
    logicalTarget_.clear(sf::Color(8, 12, 20));
    starfield_.render(logicalTarget_);
    for (const auto& element : backgroundElements_) {
        element.render(logicalTarget_);
    }
    for (const auto& enemy : enemies_) {
        enemy.render(logicalTarget_);
    }
    for (const auto& explosion : explosions_) {
        explosion.render(logicalTarget_);
    }
    for (const auto& laser : playerLasers_) {
        laser.render(logicalTarget_);
    }
    for (const auto& bullet : enemyBullets_) {
        bullet.render(logicalTarget_);
    }
    for (const auto& laser : enemyLasers_) {
        laser.render(logicalTarget_);
    }
    renderMuzzleFlash(logicalTarget_);
    player_->render(logicalTarget_);
    logicalTarget_.display();

    window_.clear(sf::Color::Black);
    window_.draw(presentationSprite_);
    renderPauseOverlay();
    renderDebugHud();
    window_.display();
}

void Game::renderPauseOverlay() {
    if (!paused_) {
        return;
    }

    auto pauseText = sf::Text(debugFont_);
    pauseText.setString("PAUSE");
    pauseText.setCharacterSize(36);
    pauseText.setFillColor(sf::Color(235, 245, 255));
    pauseText.setOutlineColor(sf::Color(8, 12, 20));
    pauseText.setOutlineThickness(2.f);

    const auto bounds = pauseText.getLocalBounds();
    pauseText.setOrigin({
        bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y + bounds.size.y * 0.5f
    });
    pauseText.setPosition({
        static_cast<float>(window_.getSize().x) * 0.5f,
        static_cast<float>(window_.getSize().y) * 0.5f
    });
    window_.draw(pauseText);
}

void Game::renderDebugHud() {
    const auto spritePosition = presentationSprite_.getPosition();
    const auto leftPanelWidth = spritePosition.x;

    if (leftPanelWidth < 96.f) {
        return;
    }

    auto text = std::ostringstream{};
    text << std::fixed << std::setprecision(2)
         << "TIME\n"
         << stageClock_.asSeconds()
         << " s\n\n"
         << std::setprecision(1)
         << "FPS\n"
         << smoothedFps_;

    debugText_.setString(text.str());
    debugText_.setPosition({24.f, 24.f});
    window_.draw(debugText_);
}

void Game::renderMuzzleFlash(sf::RenderTarget& target) const {
    if (muzzleFlashTime_ <= sf::Time::Zero || !player_ || !muzzleFlashTexture_) {
        return;
    }

    const auto spawn = player_->laserSpawnPosition();
    auto flash = sf::Sprite(*muzzleFlashTexture_);
    const auto textureSize = muzzleFlashTexture_->getSize();
    flash.setOrigin({
        static_cast<float>(textureSize.x) * 0.5f,
        static_cast<float>(textureSize.y) * 0.5f
    });
    flash.setPosition({std::round(spawn.x), std::round(spawn.y + 4.f)});
    target.draw(flash);
}

const sf::Texture* Game::bulletTextureForPattern(const std::string& patternId) const {
    const auto& bulletId = bulletPatternSystem_.bulletId(patternId);
    if (bulletId != "default" && projectileConfigSystem_.hasProjectile(bulletId)) {
        return &assets_.getTexture(bulletId);
    }

    return nullptr;
}

const sf::Texture* Game::laserTextureForPattern(const std::string& patternId) const {
    const auto& laserId = bulletPatternSystem_.laserId(patternId);
    if (laserId != "default" && projectileConfigSystem_.hasProjectile(laserId)) {
        return &assets_.getTexture(laserId);
    }

    return nullptr;
}

void Game::updatePresentationSprite() {
    const auto windowSize = window_.getSize();
    const auto scaleX = static_cast<float>(windowSize.x) / static_cast<float>(LogicalWidth);
    const auto scaleY = static_cast<float>(windowSize.y) / static_cast<float>(LogicalHeight);
    auto integerScale = 1.f;

    if (presentationScaleMode_ == PresentationScaleMode::IntegerFit) {
        integerScale = std::max(1.f, std::floor(std::min(scaleX, scaleY)));
    }

    presentationSprite_.setTexture(logicalTarget_.getTexture(), true);
    presentationSprite_.setScale({integerScale, integerScale});

    const auto scaledWidth = static_cast<float>(LogicalWidth) * integerScale;
    const auto scaledHeight = static_cast<float>(LogicalHeight) * integerScale;
    presentationSprite_.setPosition({
        (static_cast<float>(windowSize.x) - scaledWidth) * 0.5f,
        (static_cast<float>(windowSize.y) - scaledHeight) * 0.5f
    });
}
