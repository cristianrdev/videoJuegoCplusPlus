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
        5,  // RB / R1
        7   // common trigger/shoulder mapping on generic controllers
    });
}
}

Game::Game()
    : window_(sf::VideoMode({WindowWidth, WindowHeight}), "Shooter vertical")
    , logicalTarget_({LogicalWidth, LogicalHeight})
    , presentationSprite_(logicalTarget_.getTexture())
    , debugText_(debugFont_)
    , assets_("assets") {
    window_.setVerticalSyncEnabled(true);
    logicalTarget_.setSmooth(false);

    if (!debugFont_.openFromFile("C:/Windows/Fonts/consola.ttf")) {
        throw std::runtime_error("No se pudo cargar fuente debug: C:/Windows/Fonts/consola.ttf");
    }

    debugText_.setCharacterSize(16);
    debugText_.setFillColor(sf::Color(190, 220, 230));

    assets_.loadTexture("player_ship_sheet", "textures/player/player_ship_sheet_ai_transparent.png");
    laserNormalTexture_ = &assets_.loadTexture("player_laser_normal", "textures/player/player_laser_normal.png");
    muzzleFlashTexture_ = &assets_.loadTexture("player_laser_muzzle_flash", "textures/player/player_laser_muzzle_flash.png");
    enemyDroneTexture_ = &assets_.loadTexture("enemy_drone", "textures/enemies/enemy_drone.png");
    enemyTurretPodTexture_ = &assets_.loadTexture("enemy_turret_pod", "textures/enemies/enemy_turret_pod.png");
    enemyInterceptorTexture_ = &assets_.loadTexture("enemy_interceptor", "textures/enemies/enemy_interceptor.png");
    bulletPatternSystem_.loadFromFile("config/bullet_patterns.json");
    movementPatternSystem_.loadFromFile("config/movement_patterns.json");
    stageDirector_.loadFromFile("config/stage_01.json");

    player_ = std::make_unique<Player>(
        assets_,
        sf::Vector2f{static_cast<float>(LogicalWidth), static_cast<float>(LogicalHeight)}
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
    }
}

void Game::fireLaserNormal() {
    if (fireCooldown_ > sf::Time::Zero || !player_) {
        return;
    }

    playerLasers_.emplace_back(player_->laserSpawnPosition(), *laserNormalTexture_);
    fireCooldown_ = sf::seconds(0.12f);
    muzzleFlashTime_ = sf::seconds(0.06f);
}

void Game::spawnEnemy(const StageDirector::SpawnEvent& spawn) {
    const auto* texture = enemyDroneTexture_;

    if (spawn.enemyId == "enemy_turret_pod") {
        texture = enemyTurretPodTexture_;
    } else if (spawn.enemyId == "enemy_interceptor") {
        texture = enemyInterceptorTexture_;
    } else if (spawn.enemyId != "enemy_drone") {
        throw std::runtime_error("Enemigo no configurado: " + spawn.enemyId);
    }

    enemies_.emplace_back(
        sf::Vector2f{spawn.x, spawn.y},
        *texture,
        spawn.patternId,
        spawn.movementId
    );
}

void Game::update(sf::Time deltaTime) {
    stageClock_ += deltaTime;
    const auto frameSeconds = deltaTime.asSeconds();
    if (frameSeconds > 0.f) {
        const auto instantFps = 1.f / frameSeconds;
        smoothedFps_ = smoothedFps_ == 0.f ? instantFps : smoothedFps_ * 0.90f + instantFps * 0.10f;
    }

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
}

void Game::updateEnemyShooting() {
    if (!player_) {
        return;
    }

    for (auto& enemy : enemies_) {
        if (!enemy.shouldFire()) {
            continue;
        }

        auto bullets = bulletPatternSystem_.spawn(
            enemy.patternId(),
            enemy.bulletSpawnPosition(),
            player_->position()
        );
        enemyBullets_.insert(enemyBullets_.end(), bullets.begin(), bullets.end());
        enemy.resetFireTimer(bulletPatternSystem_.fireInterval(enemy.patternId()));
    }
}

void Game::updateCollisions() {
    for (auto laserIt = playerLasers_.begin(); laserIt != playerLasers_.end();) {
        auto hitEnemy = false;

        for (auto& enemy : enemies_) {
            if (intersects(laserIt->hitbox(), enemy.hitbox())) {
                enemy.takeDamage(LaserNormal::Damage);
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
}

void Game::render() {
    logicalTarget_.clear(sf::Color(8, 12, 20));
    for (const auto& enemy : enemies_) {
        enemy.render(logicalTarget_);
    }
    for (const auto& laser : playerLasers_) {
        laser.render(logicalTarget_);
    }
    for (const auto& bullet : enemyBullets_) {
        bullet.render(logicalTarget_);
    }
    renderMuzzleFlash(logicalTarget_);
    player_->render(logicalTarget_);
    logicalTarget_.display();

    window_.clear(sf::Color::Black);
    window_.draw(presentationSprite_);
    renderDebugHud();
    window_.display();
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
