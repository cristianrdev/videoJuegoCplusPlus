#include "PlayState.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
std::vector<sf::Vector2f> playerProjectileOffsets(int projectileCount) {
    switch (projectileCount) {
    case 2:
        return {{-8.f, 8.f}, {8.f, 8.f}};
    case 4:
        return {{-12.f, 9.f}, {-5.f, 2.f}, {5.f, 2.f}, {12.f, 9.f}};
    case 8:
        return {
            {-15.f, 10.f},
            {-11.f, 4.f},
            {-7.f, -2.f},
            {-3.f, -8.f},
            {3.f, -8.f},
            {7.f, -2.f},
            {11.f, 4.f},
            {15.f, 10.f}
        };
    default:
        return {{0.f, 0.f}};
    }
}
}

PlayState::PlayState(AssetManager& assets, sf::Vector2f logicalSize)
    : assets_(assets)
    , logicalSize_(logicalSize)
    , enemySpawner_(assets_, enemyConfigSystem_)
    , starfield_(logicalSize) {
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

    itemConfigSystem_.loadFromFile("config/items.json");
    for (const auto& itemId : itemConfigSystem_.textureIds()) {
        assets_.loadTexture(itemId, itemConfigSystem_.texturePathFor(itemId));
    }
    itemConfigSystem_.loadStageSpawnsFromFile("config/stage_01_items.json");

    floatingRedRocksTexture_ = &assets_.loadTexture("floating_red_rocks_tileset", "textures/background/floating_red_rocks_tileset.png");
    bulletPatternSystem_.loadFromFile("config/bullet_patterns.json");
    movementPatternSystem_.loadFromFile("config/movement_patterns.json");
    stageDirector_.loadFromFile("config/stage_01_enemies.json");
    backgroundElementDirector_.loadFromFile("config/stage_01_background_elements.json");

    player_ = std::make_unique<Player>(assets_, logicalSize_, playerConfigSystem_.config());
}

void PlayState::update(sf::Time deltaTime) {
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

    for (const auto& spawn : stageDirector_.update(deltaTime)) {
        spawnEnemy(spawn);
    }

    for (const auto& spawn : backgroundElementDirector_.update(deltaTime)) {
        spawnBackgroundElement(spawn);
    }

    const auto& itemSpawns = itemConfigSystem_.spawns();
    while (nextItemSpawnIndex_ < itemSpawns.size() && itemSpawns[nextItemSpawnIndex_].time <= stageClock_) {
        spawnItemCarrier(itemSpawns[nextItemSpawnIndex_]);
        ++nextItemSpawnIndex_;
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

    for (auto& carrier : itemCarriers_) {
        carrier.update(deltaTime);
    }

    for (auto& powerUp : powerUps_) {
        powerUp.update(deltaTime);
    }

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
            [this](const EnemyBullet& bullet) {
                return !bullet.isAlive(logicalSize_);
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
            [this](const BackgroundElement& element) {
                return !element.isAlive(logicalSize_.y);
            }
        ),
        backgroundElements_.end()
    );

    updateCollisions();
    processEvents();

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

    itemCarriers_.erase(
        std::remove_if(
            itemCarriers_.begin(),
            itemCarriers_.end(),
            [this](const ItemCarrier& carrier) {
                return !carrier.isAlive(logicalSize_);
            }
        ),
        itemCarriers_.end()
    );

    powerUps_.erase(
        std::remove_if(
            powerUps_.begin(),
            powerUps_.end(),
            [](const PowerUpItem& powerUp) {
                return !powerUp.isAlive();
            }
        ),
        powerUps_.end()
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

void PlayState::render(sf::RenderTarget& target) {
    target.clear(sf::Color(8, 12, 20));
    starfield_.render(target);
    for (const auto& element : backgroundElements_) {
        element.render(target);
    }
    for (const auto& enemy : enemies_) {
        enemy.render(target);
    }
    for (const auto& carrier : itemCarriers_) {
        carrier.render(target);
    }
    for (const auto& explosion : explosions_) {
        explosion.render(target);
    }
    for (const auto& laser : playerLasers_) {
        laser.render(target);
    }
    for (const auto& bullet : enemyBullets_) {
        bullet.render(target);
    }
    for (const auto& laser : enemyLasers_) {
        laser.render(target);
    }
    for (const auto& powerUp : powerUps_) {
        powerUp.render(target);
    }
    renderMuzzleFlash(target);
    player_->render(target);
}

void PlayState::fireLaserNormal() {
    if (fireCooldown_ > sf::Time::Zero || !player_) {
        return;
    }

    const auto& config = playerConfigSystem_.config();
    const auto spawn = player_->laserSpawnPosition();
    for (const auto offset : playerProjectileOffsets(player_->projectileCount())) {
        playerLasers_.emplace_back(
            sf::Vector2f{spawn.x + offset.x, spawn.y + offset.y},
            *laserNormalTexture_,
            config.laserSpeed,
            config.laserDamage
        );
    }
    fireCooldown_ = sf::seconds(config.fireCooldownSeconds);
    muzzleFlashTime_ = sf::seconds(config.muzzleFlashSeconds);
}

void PlayState::onPaused() {
    muzzleFlashTime_ = sf::Time::Zero;
}

sf::Time PlayState::stageTime() const {
    return stageClock_;
}

void PlayState::spawnEnemy(const StageDirector::SpawnEvent& spawn) {
    enemies_.push_back(enemySpawner_.spawn(spawn));
}

void PlayState::spawnBackgroundElement(const BackgroundElementDirector::SpawnEvent& spawn) {
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

void PlayState::spawnItemCarrier(const ItemSpawnConfig& spawn) {
    const auto& config = itemConfigSystem_.carrierConfigFor(spawn.itemId);
    itemCarriers_.emplace_back(
        sf::Vector2f{spawn.x, spawn.y},
        assets_.getTexture(spawn.itemId),
        config
    );
}

void PlayState::spawnPowerUp(const std::string& powerUpId, sf::Vector2f position) {
    const auto& config = itemConfigSystem_.powerUpConfigFor(powerUpId);
    powerUps_.emplace_back(position, assets_.getTexture(powerUpId), config);
}

void PlayState::spawnExplosion(const std::string& enemyId, sf::Vector2f position) {
    auto texture = explosionDroneTexture_;

    if (enemyId == "enemy_turret_pod") {
        texture = explosionTurretPodTexture_;
    } else if (enemyId == "enemy_interceptor") {
        texture = explosionInterceptorTexture_;
    }

    explosions_.emplace_back(position, *texture);
}

void PlayState::updateEnemyShooting() {
    if (!player_) {
        return;
    }

    for (auto& enemy : enemies_) {
        if (!enemy.shouldFire()) {
            continue;
        }

        if (bulletPatternSystem_.patternType(enemy.patternId()) == "continuous_laser") {
            const auto origin = enemy.bulletSpawnPosition();
            const auto length = std::max(0.f, logicalSize_.y - origin.y);
            const auto duration = sf::seconds(bulletPatternSystem_.laserDuration(enemy.patternId()));
            const auto* laserTexture = laserTextureForPattern(enemy.patternId());
            if (length > 0.f && laserTexture) {
                enemyLasers_.emplace_back(origin, length, duration, *laserTexture);
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

void PlayState::updateCollisions() {
    if (!player_) {
        return;
    }

    collisionSystem_.resolve(
        playerLasers_,
        enemies_,
        enemyBullets_,
        enemyLasers_,
        *player_,
        eventQueue_
    );

    collisionSystem_.resolveItems(
        playerLasers_,
        itemCarriers_,
        powerUps_,
        *player_,
        logicalSize_,
        eventQueue_
    );
}

void PlayState::processEvents() {
    for (const auto& event : eventQueue_.drain()) {
        if (const auto* enemyDestroyed = std::get_if<EnemyDestroyedEvent>(&event)) {
            spawnExplosion(enemyDestroyed->enemyId, enemyDestroyed->position);
        } else if (const auto* itemDestroyed = std::get_if<ItemCarrierDestroyedEvent>(&event)) {
            spawnPowerUp(itemDestroyed->dropId, itemDestroyed->position);
        } else if (const auto* powerUpCollected = std::get_if<PowerUpCollectedEvent>(&event)) {
            if (powerUpCollected->powerUpId == "power_p") {
                player_->collectPowerUpP();
            }
        }
    }
}

void PlayState::renderMuzzleFlash(sf::RenderTarget& target) const {
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

const sf::Texture* PlayState::bulletTextureForPattern(const std::string& patternId) const {
    const auto& bulletId = bulletPatternSystem_.bulletId(patternId);
    if (bulletId != "default" && projectileConfigSystem_.hasProjectile(bulletId)) {
        return &assets_.getTexture(bulletId);
    }

    return nullptr;
}

const sf::Texture* PlayState::laserTextureForPattern(const std::string& patternId) const {
    const auto& laserId = bulletPatternSystem_.laserId(patternId);
    if (laserId != "default" && projectileConfigSystem_.hasProjectile(laserId)) {
        return &assets_.getTexture(laserId);
    }

    return nullptr;
}
