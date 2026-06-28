#include "PlayState.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
constexpr auto PlayerDeathExplosionSeconds = 0.21f;
constexpr auto GameOverDelaySeconds = 3.0f;
constexpr auto StarfieldFadeStartSeconds = 64.0f;
constexpr auto StarfieldFadeDurationSeconds = 6.0f;
constexpr auto Pi = 3.14159265358979323846f;

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

float rotationDegreesFacingTarget(sf::Vector2f from, sf::Vector2f target) {
    const auto delta = target - from;
    if (std::abs(delta.x) <= 0.001f && std::abs(delta.y) <= 0.001f) {
        return 0.f;
    }

    return -std::atan2(delta.x, delta.y) * 180.f / Pi;
}

float starfieldOpacityForStageTime(sf::Time stageTime) {
    const auto fadeElapsed = stageTime.asSeconds() - StarfieldFadeStartSeconds;
    if (fadeElapsed <= 0.f) {
        return 1.f;
    }

    return 1.f - std::clamp(fadeElapsed / StarfieldFadeDurationSeconds, 0.f, 1.f);
}

float length(sf::Vector2f value) {
    return std::sqrt(value.x * value.x + value.y * value.y);
}

sf::Vector2f normalizedOr(sf::Vector2f value, sf::Vector2f fallback) {
    const auto valueLength = length(value);
    if (valueLength <= 0.001f) {
        const auto fallbackLength = length(fallback);
        if (fallbackLength <= 0.001f) {
            return {0.f, -1.f};
        }
        return {fallback.x / fallbackLength, fallback.y / fallbackLength};
    }

    return {value.x / valueLength, value.y / valueLength};
}

void drawLine(sf::RenderTarget& target, sf::Vector2f from, sf::Vector2f to, sf::Color color, float thickness = 1.f) {
    const auto delta = to - from;
    const auto lineLength = length(delta);
    if (lineLength <= 0.001f) {
        return;
    }

    auto line = sf::RectangleShape({lineLength, thickness});
    line.setOrigin({0.f, thickness * 0.5f});
    line.setPosition({std::round(from.x), std::round(from.y)});
    line.setRotation(sf::degrees(std::atan2(delta.y, delta.x) * 180.f / Pi));
    line.setFillColor(color);
    target.draw(line);
}
}

PlayState::PlayState(AssetManager& assets, sf::Vector2f logicalSize, sf::Time initialStageTime)
    : assets_(assets)
    , logicalSize_(logicalSize)
    , enemySpawner_(assets_, enemyConfigSystem_)
    , starfield_(logicalSize) {
    playerConfigSystem_.loadFromFile("config/player.json");
    lockOnConfigSystem_.loadFromFile("config/player_lock_on.json");
    const auto& playerConfig = playerConfigSystem_.config();
    assets_.loadTexture("player_ship_sheet", playerConfig.shipTexture);
    assets_.loadTexture("player_thruster_flame", playerConfig.thrusterTexture);
    laserNormalTexture_ = &assets_.loadTexture("player_laser_normal", playerConfig.laserTexture);
    muzzleFlashTexture_ = &assets_.loadTexture("player_laser_muzzle_flash", playerConfig.muzzleFlashTexture);

    enemyConfigSystem_.loadFromFile("config/enemies.json");
    for (const auto& enemyId : enemyConfigSystem_.enemyIds()) {
        assets_.loadTexture(enemyId, enemyConfigSystem_.texturePathFor(enemyId));
    }

    wallBossConfigSystem_.loadFromFile("config/wall_bosses.json");
    for (const auto& wallBossId : wallBossConfigSystem_.wallBossIds()) {
        assets_.loadTexture(wallBossId, wallBossConfigSystem_.texturePathFor(wallBossId));
    }

    explosionDroneTexture_ = &assets_.loadTexture("explosion_enemy_drone", "textures/effects/explosion_enemy_drone.png");
    explosionTurretPodTexture_ = &assets_.loadTexture("explosion_enemy_turret_pod", "textures/effects/explosion_enemy_turret_pod.png");
    explosionInterceptorTexture_ = &assets_.loadTexture("explosion_enemy_interceptor", "textures/effects/explosion_enemy_interceptor.png");
    explosionGreenCargoTankTexture_ = &assets_.loadTexture("explosion_enemy_green_cargo_tank", "textures/effects/explosion_enemy_green_cargo_tank.png");
    explosionMechanicalSpikedShellTexture_ = &assets_.loadTexture("explosion_enemy_mechanical_spiked_shell", "textures/effects/explosion_enemy_mechanical_spiked_shell.png");
    explosionArmoredFlailShipTexture_ = &assets_.loadTexture("explosion_enemy_armored_flail_ship", "textures/effects/explosion_enemy_armored_flail_ship.png");
    playerExplosionTexture_ = &assets_.loadTexture("explosion_player_ship", "textures/effects/player_ship_destroy_explosion.png");
    enemyHitSparkTexture_ = &assets_.loadTexture("enemy_hit_spark", "textures/effects/enemy_hit_spark.png");

    projectileConfigSystem_.loadFromFile("config/projectiles.json");
    for (const auto& projectileId : projectileConfigSystem_.projectileIds()) {
        if (projectileConfigSystem_.hasTexture(projectileId)) {
            assets_.loadTexture(projectileId, projectileConfigSystem_.texturePathFor(projectileId));
        }
    }

    itemConfigSystem_.loadFromFile("config/items.json");
    for (const auto& itemId : itemConfigSystem_.textureIds()) {
        assets_.loadTexture(itemId, itemConfigSystem_.texturePathFor(itemId));
    }
    itemConfigSystem_.loadStageSpawnsFromFile("config/stage_01_items.json");

    floatingRedRocksTexture_ = &assets_.loadTexture("floating_red_rocks_tileset", "textures/background/floating_red_rocks_tileset.png");
    earthLikePlanetTexture_ = &assets_.loadTexture("earth_like_planet_horizon", "textures/background/earth_like_planet_horizon.png");
    backgroundElementConfigSystem_.loadFromFile("config/background_elements.json");
    bulletPatternSystem_.loadFromFile("config/bullet_patterns.json");
    movementPatternSystem_.loadFromFile("config/movement_patterns.json");
    checkpointSystem_.loadFromFile("config/stage_01_checkpoint.json");
    stageDirector_.loadFromFile("config/stage_01_enemies.json");
    backgroundElementDirector_.loadFromFile("config/stage_01_background_elements.json");
    stageClock_ = initialStageTime;
    stageDirector_.seekTo(initialStageTime);
    backgroundElementDirector_.seekTo(initialStageTime);
    while (nextItemSpawnIndex_ < itemConfigSystem_.spawns().size() &&
           itemConfigSystem_.spawns()[nextItemSpawnIndex_].time <= initialStageTime) {
        ++nextItemSpawnIndex_;
    }

    player_ = std::make_unique<Player>(assets_, logicalSize_, playerConfigSystem_.config());
}

void PlayState::update(sf::Time deltaTime) {
    if (playerDestroyed_) {
        playerDeathElapsed_ += deltaTime;
        starfield_.update(deltaTime);
        for (auto& bullet : enemyBullets_) {
            bullet.update(deltaTime);
        }
        for (auto& explosion : explosions_) {
            explosion.update(deltaTime);
        }
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
        return;
    }

    stageClock_ += deltaTime;
    starfield_.setOpacity(starfieldOpacityForStageTime(stageClock_));

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
    for (auto& wallBoss : wallBosses_) {
        wallBoss.update(deltaTime);
    }

    player_->update(deltaTime);
    updateLockOn(deltaTime);

    for (auto& laser : playerLasers_) {
        laser.update(deltaTime);
    }
    updateHomingLasers(deltaTime);

    for (auto& enemy : enemies_) {
        enemy.update(deltaTime);
        if (movementPatternSystem_.isApproachHoldRetreat(enemy.movementId())) {
            if (movementPatternSystem_.shouldLockTargetOnStart(enemy.movementId()) &&
                !enemy.hasMovementTargetPosition()) {
                enemy.lockMovementTargetPosition(player_->position());
            }

            const auto movementTargetPosition = enemy.hasMovementTargetPosition()
                ? enemy.movementTargetPosition()
                : player_->position();

            if (!enemy.hasMovementHoldPosition()) {
                const auto nextPosition = movementPatternSystem_.positionFor(
                    enemy.movementId(),
                    enemy.startPosition(),
                    enemy.elapsed(),
                    movementTargetPosition
                );
                enemy.setPosition(nextPosition);
                if (movementPatternSystem_.canFire(
                        enemy.movementId(),
                        enemy.startPosition(),
                        enemy.elapsed(),
                        movementTargetPosition
                    )) {
                    enemy.lockMovementHoldPosition(nextPosition);
                }
            } else {
                const auto holdElapsed = enemy.elapsed() - enemy.movementHoldElapsed();
                if (movementPatternSystem_.isHoldFinished(enemy.movementId(), holdElapsed)) {
                    enemy.setPosition(movementPatternSystem_.retreatPositionFor(
                        enemy.movementId(),
                        enemy.movementHoldPosition(),
                        holdElapsed
                    ));
                } else {
                    enemy.setPosition(enemy.movementHoldPosition());
                }
            }

            enemy.setRotationDegrees(rotationDegreesFacingTarget(enemy.position(), player_->position()));
            continue;
        }

        enemy.setRotationDegrees(0.f);
        enemy.setPosition(
            movementPatternSystem_.positionFor(
                enemy.movementId(),
                enemy.startPosition(),
                enemy.elapsed(),
                player_->position()
            )
        );
    }

    updateEnemyShooting();
    updateWallBossShooting();

    for (auto& bullet : enemyBullets_) {
        if (!bullet.isPixelLine() && !bullet.followsOwnerAnchor()) {
            continue;
        }

        auto anchorFound = false;
        for (const auto& enemy : enemies_) {
            if (enemy.instanceId() == bullet.ownerInstanceId() && enemy.isAlive()) {
                if (bullet.followsOwnerAnchor()) {
                    bullet.setTetherOrigin(enemy.bulletSpawnPosition());
                } else {
                    bullet.setPosition(enemy.bulletSpawnPosition());
                }
                anchorFound = true;
                break;
            }
        }

        if (anchorFound) {
            continue;
        }

        for (const auto& wallBoss : wallBosses_) {
            auto alive = false;
            const auto crystalPosition = wallBoss.crystalPositionForOwner(bullet.ownerInstanceId(), alive);
            if (!alive) {
                continue;
            }

            if (bullet.followsOwnerAnchor()) {
                bullet.setTetherOrigin(crystalPosition);
            } else {
                bullet.setPosition(crystalPosition);
            }
            break;
        }
    }

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

    homingLasers_.erase(
        std::remove_if(
            homingLasers_.begin(),
            homingLasers_.end(),
            [this](const PlayerHomingLaser& laser) {
                return !laser.isAlive(logicalSize_);
            }
        ),
        homingLasers_.end()
    );

    enemyBullets_.erase(
        std::remove_if(
            enemyBullets_.begin(),
            enemyBullets_.end(),
            [this](const EnemyBullet& bullet) {
                if (bullet.followsOwnerAnchor() && !isBulletOwnerAlive(bullet.ownerInstanceId())) {
                    return true;
                }
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

    wallBosses_.erase(
        std::remove_if(
            wallBosses_.begin(),
            wallBosses_.end(),
            [this](const WallBoss& wallBoss) {
                return !wallBoss.isAlive(logicalSize_.y);
            }
        ),
        wallBosses_.end()
    );

    updateCollisions();
    resolveWallBossCollisions();
    resolveHomingLaserCollisions();
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
    const auto showDebugHitboxes = isPlayerHitboxVisible();
    for (const auto& element : backgroundElements_) {
        element.render(target, showDebugHitboxes);
    }
    for (const auto& wallBoss : wallBosses_) {
        wallBoss.render(target, showDebugHitboxes);
    }
    for (const auto& enemy : enemies_) {
        enemy.render(target, showDebugHitboxes);
    }
    renderLockOnField(target);
    renderLockOnTargets(target);
    for (const auto& carrier : itemCarriers_) {
        carrier.render(target);
    }
    for (const auto& explosion : explosions_) {
        explosion.render(target);
    }
    for (const auto& laser : playerLasers_) {
        laser.render(target);
    }
    for (const auto& laser : homingLasers_) {
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
    if (!playerDestroyed_) {
        player_->render(target);
    }
    renderLockOnCharge(target);
}

void PlayState::fireLaserNormal() {
    if (playerDestroyed_ || fireCooldown_ > sf::Time::Zero || !player_) {
        return;
    }

    const auto& config = playerConfigSystem_.config();
    const auto spawn = player_->laserSpawnPosition();
    const auto projectileCount = player_->projectileCount();
    const auto projectileDamage = config.laserDamageForProjectileCount(projectileCount);
    for (const auto offset : playerProjectileOffsets(projectileCount)) {
        playerLasers_.emplace_back(
            sf::Vector2f{spawn.x + offset.x, spawn.y + offset.y},
            *laserNormalTexture_,
            config.laserSpeed,
            projectileDamage
        );
    }
    fireCooldown_ = sf::seconds(config.fireCooldownSeconds);
    muzzleFlashTime_ = sf::seconds(config.muzzleFlashSeconds);
}

void PlayState::setFireButtonPressed(bool pressed) {
    fireButtonPressed_ = pressed;
    if (playerDestroyed_ || !player_) {
        wasFireButtonPressed_ = pressed;
        return;
    }

    if (!pressed && wasFireButtonPressed_) {
        lockOnChargeStarted_ = true;
        lockOnCharge_ = sf::Time::Zero;
        lockOnTargetIds_.clear();
    }

    const auto& config = lockOnConfigSystem_.config();
    const auto chargeComplete = lockOnCharge_.asSeconds() >= config.chargeSeconds;
    if (pressed && !wasFireButtonPressed_ && chargeComplete && !lockOnTargetIds_.empty()) {
        launchLockOnLasers();
        lockOnCharge_ = sf::Time::Zero;
        lockOnTargetIds_.clear();
        lockOnChargeStarted_ = false;
        wasFireButtonPressed_ = pressed;
        return;
    }

    if (pressed) {
        lockOnChargeStarted_ = false;
        fireLaserNormal();
    }

    wasFireButtonPressed_ = pressed;
}

void PlayState::onPaused() {
    muzzleFlashTime_ = sf::Time::Zero;
}

sf::Time PlayState::stageTime() const {
    return stageClock_;
}

int PlayState::activeCheckpointIndex() const {
    return checkpointSystem_.activeCheckpointIndex(stageClock_);
}

sf::Time PlayState::activeCheckpointTime() const {
    return checkpointSystem_.activeCheckpointTime(stageClock_);
}

bool PlayState::isPlayerDestroyed() const {
    return playerDestroyed_;
}

bool PlayState::isGameOverVisible() const {
    return playerDestroyed_ &&
        playerDeathElapsed_ >= sf::seconds(PlayerDeathExplosionSeconds + GameOverDelaySeconds);
}

int PlayState::playerHealth() const {
    return player_ ? player_->health() : 0;
}

bool PlayState::isGodModeEnabled() const {
    return player_ && player_->isGodModeEnabled();
}

void PlayState::toggleGodMode() {
    if (!player_ || playerDestroyed_) {
        return;
    }

    player_->setGodModeEnabled(!player_->isGodModeEnabled());
}

bool PlayState::isPlayerHitboxVisible() const {
    return player_ && player_->isHitboxVisible();
}

void PlayState::togglePlayerHitbox() {
    if (!player_) {
        return;
    }

    player_->setHitboxVisible(!player_->isHitboxVisible());
}

void PlayState::updateLockOn(sf::Time deltaTime) {
    if (!player_ || playerDestroyed_) {
        return;
    }

    const auto& config = lockOnConfigSystem_.config();
    if (!lockOnChargeStarted_) {
        lockOnCharge_ = sf::Time::Zero;
        lockOnTargetIds_.clear();
        return;
    }

    if (fireButtonPressed_) {
        lockOnCharge_ = sf::Time::Zero;
        lockOnTargetIds_.clear();
        lockOnChargeStarted_ = false;
        return;
    }

    if (lockOnCharge_.asSeconds() < config.chargeSeconds) {
        lockOnCharge_ += deltaTime;
        if (lockOnCharge_.asSeconds() > config.chargeSeconds) {
            lockOnCharge_ = sf::seconds(config.chargeSeconds);
        }
        lockOnTargetIds_.clear();
        return;
    }

    lockOnTargetIds_.erase(
        std::remove_if(
            lockOnTargetIds_.begin(),
            lockOnTargetIds_.end(),
            [this](int targetId) {
                auto alive = false;
                positionForEnemyInstance(targetId, alive);
                return !alive;
            }
        ),
        lockOnTargetIds_.end()
    );

    for (const auto& enemy : enemies_) {
        if (!enemy.isAlive() || !enemyInsideLockOnField(enemy)) {
            continue;
        }

        const auto alreadyMarked = std::find(
            lockOnTargetIds_.begin(),
            lockOnTargetIds_.end(),
            enemy.instanceId()
        ) != lockOnTargetIds_.end();
        if (alreadyMarked) {
            continue;
        }

        lockOnTargetIds_.push_back(enemy.instanceId());
        if (static_cast<int>(lockOnTargetIds_.size()) >= config.maxTargets) {
            break;
        }
    }

    for (const auto& wallBoss : wallBosses_) {
        if (static_cast<int>(lockOnTargetIds_.size()) >= config.maxTargets) {
            break;
        }

        for (const auto& crystal : wallBoss.crystals()) {
            if (!crystal.alive || !positionInsideLockOnField(crystal.position)) {
                continue;
            }

            const auto alreadyMarked = std::find(
                lockOnTargetIds_.begin(),
                lockOnTargetIds_.end(),
                crystal.instanceId
            ) != lockOnTargetIds_.end();
            if (alreadyMarked) {
                continue;
            }

            lockOnTargetIds_.push_back(crystal.instanceId);
            if (static_cast<int>(lockOnTargetIds_.size()) >= config.maxTargets) {
                break;
            }
        }
    }
}

void PlayState::updateHomingLasers(sf::Time deltaTime) {
    for (auto& laser : homingLasers_) {
        auto targetAlive = false;
        const auto targetPosition = positionForEnemyInstance(laser.targetInstanceId(), targetAlive);
        laser.update(deltaTime, targetPosition, targetAlive);
    }
}

void PlayState::resolveHomingLaserCollisions() {
    for (auto& laser : homingLasers_) {
        if (!laser.isAlive(logicalSize_)) {
            continue;
        }

        for (auto& enemy : enemies_) {
            if (!enemy.isAlive()) {
                continue;
            }

            if (laser.targetInstanceId() != enemy.instanceId() && !enemy.intersects(laser.hitbox())) {
                continue;
            }

            if (!enemy.intersects(laser.hitbox())) {
                continue;
            }

            eventQueue_.publish(EnemyHitEvent{enemy.position()});
            enemy.takeDamage(laser.damage());
            if (!enemy.isAlive()) {
                eventQueue_.publish(EnemyDestroyedEvent{
                    enemy.enemyId(),
                    enemy.patternId(),
                    enemy.instanceId(),
                    enemy.position()
                });
            }
            laser.destroy();
            break;
        }

        if (!laser.isAlive(logicalSize_)) {
            continue;
        }

        for (auto& wallBoss : wallBosses_) {
            const auto result = wallBoss.damageCrystal(
                laser.hitbox(),
                laser.damage(),
                laser.targetInstanceId()
            );
            if (!result.hit) {
                continue;
            }

            eventQueue_.publish(EnemyHitEvent{result.position});
            if (result.destroyed) {
                spawnWallBossCrystalExplosion(result.position);
            }
            laser.destroy();
            break;
        }
    }
}

bool PlayState::enemyInsideLockOnField(const Enemy& enemy) const {
    return positionInsideLockOnField(enemy.position());
}

bool PlayState::positionInsideLockOnField(sf::Vector2f position) const {
    if (!player_) {
        return false;
    }

    const auto& config = lockOnConfigSystem_.config();
    const auto origin = player_->position();
    const auto delta = position - origin;
    if (delta.y >= 0.f) {
        return false;
    }

    const auto forwardDistance = -delta.y;
    if (forwardDistance > config.fieldRange) {
        return false;
    }

    const auto halfAngle = config.fieldAngleDegrees * 0.5f * Pi / 180.f;
    const auto halfWidthAtY = std::tan(halfAngle) * forwardDistance;
    return std::abs(delta.x) <= halfWidthAtY;
}

sf::Vector2f PlayState::positionForEnemyInstance(int enemyInstanceId, bool& alive) const {
    for (const auto& enemy : enemies_) {
        if (enemy.instanceId() == enemyInstanceId && enemy.isAlive()) {
            alive = true;
            return enemy.position();
        }
    }

    for (const auto& wallBoss : wallBosses_) {
        const auto position = wallBoss.crystalPositionForOwner(enemyInstanceId, alive);
        if (alive) {
            return position;
        }
    }

    alive = false;
    return {logicalSize_.x * 0.5f, -16.f};
}

bool PlayState::isBulletOwnerAlive(int ownerInstanceId) const {
    for (const auto& enemy : enemies_) {
        if (enemy.instanceId() == ownerInstanceId && enemy.isAlive()) {
            return true;
        }
    }

    for (const auto& wallBoss : wallBosses_) {
        if (wallBoss.isCrystalOwnerAlive(ownerInstanceId)) {
            return true;
        }
    }

    return false;
}

void PlayState::launchLockOnLasers() {
    if (!player_) {
        return;
    }

    const auto& config = lockOnConfigSystem_.config();
    const auto origin = player_->position();
    const auto hitboxSize = sf::Vector2f{config.missileHitboxWidth, config.missileHitboxHeight};
    const auto visualSize = sf::Vector2f{config.missileVisualLength, config.missileVisualWidth};

    for (const auto targetId : lockOnTargetIds_) {
        homingLasers_.emplace_back(
            sf::Vector2f{origin.x - 12.f, origin.y - 2.f},
            targetId,
            -1.f,
            config.missileSpeed,
            config.missileTurnRate,
            config.missileSideExitSeconds,
            config.missileSideExitSpeed,
            config.missileDamage,
            hitboxSize,
            visualSize,
            config.missileCoreWidth,
            config.missileGlowRadius
        );
        homingLasers_.emplace_back(
            sf::Vector2f{origin.x + 12.f, origin.y - 2.f},
            targetId,
            1.f,
            config.missileSpeed,
            config.missileTurnRate,
            config.missileSideExitSeconds,
            config.missileSideExitSpeed,
            config.missileDamage,
            hitboxSize,
            visualSize,
            config.missileCoreWidth,
            config.missileGlowRadius
        );
    }
}

void PlayState::spawnEnemy(const StageDirector::SpawnEvent& spawn) {
    if (wallBossConfigSystem_.hasWallBoss(spawn.enemyId)) {
        wallBosses_.emplace_back(
            sf::Vector2f{spawn.x, spawn.y},
            assets_.getTexture(spawn.enemyId),
            wallBossConfigSystem_.configFor(spawn.enemyId)
        );
        return;
    }

    enemies_.push_back(enemySpawner_.spawn(spawn));
}

void PlayState::spawnBackgroundElement(const BackgroundElementDirector::SpawnEvent& spawn) {
    const auto* texture = floatingRedRocksTexture_;
    auto tileSize = sf::Vector2i{100, 100};

    if (spawn.tilesetId == "earth_like_planet") {
        texture = earthLikePlanetTexture_;
        tileSize = sf::Vector2i{240, 320};
    } else if (spawn.tilesetId != "floating_red_rocks") {
        throw std::runtime_error("Tileset de fondo no configurado: " + spawn.tilesetId);
    }

    const auto elementConfig = backgroundElementConfigSystem_.configFor(spawn.tilesetId, spawn.tileIndex);
    backgroundElements_.emplace_back(
        sf::Vector2f{spawn.x, spawn.y},
        spawn.speedY,
        *texture,
        spawn.tileIndex,
        tileSize,
        elementConfig.hitboxShape,
        elementConfig.hitboxOffset,
        elementConfig.hitboxSize,
        spawn.contactDamage
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
    if (config.transformTo != "none") {
        powerUps_.emplace_back(
            position,
            assets_.getTexture(powerUpId),
            config,
            assets_.getTexture(config.transformTo),
            config.transformTo
        );
        return;
    }

    powerUps_.emplace_back(position, assets_.getTexture(powerUpId), config);
}

void PlayState::spawnExplosion(const std::string& enemyId, sf::Vector2f position) {
    if (enemyId == "enemy_green_cargo_tank" && explosionGreenCargoTankTexture_) {
        explosions_.emplace_back(
            position,
            *explosionGreenCargoTankTexture_,
            sf::Vector2i{64, 96},
            3,
            sf::seconds(0.08f)
        );
        return;
    }

    if (enemyId == "enemy_mechanical_spiked_shell" && explosionMechanicalSpikedShellTexture_) {
        explosions_.emplace_back(
            position,
            *explosionMechanicalSpikedShellTexture_,
            sf::Vector2i{48, 48},
            3,
            sf::seconds(0.07f)
        );
        return;
    }

    if (enemyId == "enemy_armored_flail_ship" && explosionArmoredFlailShipTexture_) {
        explosions_.emplace_back(
            position,
            *explosionArmoredFlailShipTexture_,
            sf::Vector2i{64, 80},
            3,
            sf::seconds(0.08f)
        );
        return;
    }

    auto texture = explosionDroneTexture_;

    if (enemyId == "enemy_turret_pod") {
        texture = explosionTurretPodTexture_;
    } else if (enemyId == "enemy_interceptor") {
        texture = explosionInterceptorTexture_;
    }

    explosions_.emplace_back(position, *texture);
}

void PlayState::spawnWallBossCrystalExplosion(sf::Vector2f position) {
    const auto texture = explosionInterceptorTexture_ ? explosionInterceptorTexture_ : explosionDroneTexture_;
    if (!texture) {
        return;
    }

    explosions_.emplace_back(position, *texture);
}

void PlayState::destroyMountedCargoSphere(sf::Vector2f tankPosition) {
    for (auto& enemy : enemies_) {
        if (!enemy.isAlive() || enemy.enemyId() != "enemy_cargo_sphere_turret") {
            continue;
        }

        const auto delta = enemy.position() - tankPosition;
        const auto distanceSquared = delta.x * delta.x + delta.y * delta.y;
        if (distanceSquared > 24.f * 24.f) {
            continue;
        }

        enemy.takeDamage(9999);
        spawnExplosion(enemy.enemyId(), enemy.position());

        if (enemy.enemyId() != "enemy_cargo_sphere_turret" &&
            bulletPatternSystem_.clearBulletsOnOwnerDestroyed(enemy.patternId())) {
            enemyBullets_.erase(
                std::remove_if(
                    enemyBullets_.begin(),
                    enemyBullets_.end(),
                    [&enemy](const EnemyBullet& bullet) {
                        return bullet.ownerInstanceId() == enemy.instanceId();
                    }
                ),
                enemyBullets_.end()
            );
        }
        return;
    }
}

void PlayState::updateEnemyShooting() {
    if (!player_) {
        return;
    }

    for (auto& enemy : enemies_) {
        const auto shouldFirePrimary = enemy.shouldFire();
        const auto shouldFireSecondary = enemy.shouldFireSecondary();
        if (!shouldFirePrimary && !shouldFireSecondary) {
            continue;
        }

        if (movementPatternSystem_.isApproachHoldRetreat(enemy.movementId())) {
            if (!enemy.hasMovementHoldPosition() ||
                movementPatternSystem_.isHoldFinished(enemy.movementId(), enemy.elapsed() - enemy.movementHoldElapsed())) {
                continue;
            }
        } else if (!movementPatternSystem_.canFire(
                       enemy.movementId(),
                       enemy.startPosition(),
                       enemy.elapsed(),
                       player_->position()
                   )) {
            continue;
        }

        if (shouldFirePrimary) {
            spawnEnemyPattern(enemy, enemy.patternId(), enemy.bulletSpawnPosition());
            enemy.resetFireTimer(bulletPatternSystem_.fireInterval(enemy.patternId()));
        }

        if (shouldFireSecondary) {
            spawnEnemyPattern(enemy, enemy.secondaryPatternId(), enemy.secondaryBulletSpawnPosition());
            enemy.resetSecondaryFireTimer(bulletPatternSystem_.fireInterval(enemy.secondaryPatternId()));
        }
    }
}

void PlayState::updateWallBossShooting() {
    if (!player_) {
        return;
    }

    for (auto& wallBoss : wallBosses_) {
        for (auto crystalIndex = std::size_t{0}; crystalIndex < wallBoss.crystalCount(); ++crystalIndex) {
            if (!wallBoss.shouldFire(crystalIndex)) {
                continue;
            }

            const auto crystal = wallBoss.crystal(crystalIndex);
            if (!crystal.alive) {
                continue;
            }

            spawnEnemyPatternForOwner(crystal.patternId, crystal.position, crystal.instanceId);
            wallBoss.resetFireTimer(crystalIndex, bulletPatternSystem_.fireInterval(crystal.patternId));
        }
    }
}

void PlayState::spawnEnemyPattern(Enemy& enemy, const std::string& patternId, sf::Vector2f origin) {
    if (bulletPatternSystem_.patternType(patternId) == "continuous_laser") {
        const auto length = std::max(0.f, logicalSize_.y - origin.y);
        const auto duration = sf::seconds(bulletPatternSystem_.laserDuration(patternId));
        const auto* laserTexture = laserTextureForPattern(patternId);
        if (length > 0.f && laserTexture) {
            enemyLasers_.emplace_back(origin, length, duration, *laserTexture, laserDamageForPattern(patternId));
            enemy.startFiringVisual(duration);
        }
        return;
    }

    auto bullets = bulletPatternSystem_.spawn(
        patternId,
        origin,
        player_->position(),
        bulletTextureForPattern(patternId),
        bulletDamageForPattern(patternId),
        bulletVisualTypeForPattern(patternId),
        bulletVisualSizeForPattern(patternId),
        bulletVisualGrowSecondsForPattern(patternId),
        enemy.instanceId(),
        bulletRotateToVelocityForPattern(patternId)
    );
    enemyBullets_.insert(enemyBullets_.end(), bullets.begin(), bullets.end());
}

void PlayState::spawnEnemyPatternForOwner(const std::string& patternId, sf::Vector2f origin, int ownerInstanceId) {
    auto bullets = bulletPatternSystem_.spawn(
        patternId,
        origin,
        player_->position(),
        bulletTextureForPattern(patternId),
        bulletDamageForPattern(patternId),
        bulletVisualTypeForPattern(patternId),
        bulletVisualSizeForPattern(patternId),
        bulletVisualGrowSecondsForPattern(patternId),
        ownerInstanceId,
        bulletRotateToVelocityForPattern(patternId)
    );
    enemyBullets_.insert(enemyBullets_.end(), bullets.begin(), bullets.end());
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

    collisionSystem_.resolveBackgroundElements(
        backgroundElements_,
        *player_,
        eventQueue_
    );
}

void PlayState::resolveWallBossCollisions() {
    if (!player_) {
        return;
    }

    for (auto laserIt = playerLasers_.begin(); laserIt != playerLasers_.end();) {
        auto hitCrystal = false;
        for (auto& wallBoss : wallBosses_) {
            const auto result = wallBoss.damageCrystal(laserIt->hitbox(), laserIt->damage());
            if (!result.hit) {
                continue;
            }

            eventQueue_.publish(EnemyHitEvent{result.position});
            if (result.destroyed) {
                spawnWallBossCrystalExplosion(result.position);
                if (bulletPatternSystem_.clearBulletsOnOwnerDestroyed(result.patternId)) {
                    enemyBullets_.erase(
                        std::remove_if(
                            enemyBullets_.begin(),
                            enemyBullets_.end(),
                            [&result](const EnemyBullet& bullet) {
                                return bullet.ownerInstanceId() == result.instanceId;
                            }
                        ),
                        enemyBullets_.end()
                    );
                }
            }

            hitCrystal = true;
            break;
        }

        if (hitCrystal) {
            laserIt = playerLasers_.erase(laserIt);
        } else {
            ++laserIt;
        }
    }

    for (const auto& wallBoss : wallBosses_) {
        if (wallBoss.contactDamage() <= 0 || !wallBoss.intersectsSolid(player_->hitbox())) {
            continue;
        }

        if (player_->takeDamage(wallBoss.contactDamage())) {
            eventQueue_.publish(PlayerHitEvent{wallBoss.contactDamage(), player_->health()});
            if (!player_->isAlive()) {
                eventQueue_.publish(PlayerDestroyedEvent{});
            }
        }
    }
}

void PlayState::processEvents() {
    for (const auto& event : eventQueue_.drain()) {
        if (const auto* enemyDestroyed = std::get_if<EnemyDestroyedEvent>(&event)) {
            spawnExplosion(enemyDestroyed->enemyId, enemyDestroyed->position);
            if (enemyDestroyed->enemyId == "enemy_green_cargo_tank") {
                destroyMountedCargoSphere(enemyDestroyed->position);
            }
            const auto secondaryPattern = enemyConfigSystem_.secondaryPatternFor(enemyDestroyed->enemyId);
            const auto clearPrimary = bulletPatternSystem_.clearBulletsOnOwnerDestroyed(enemyDestroyed->patternId);
            const auto clearSecondary = secondaryPattern != "none" &&
                bulletPatternSystem_.clearBulletsOnOwnerDestroyed(secondaryPattern);
            if (enemyDestroyed->enemyId != "enemy_cargo_sphere_turret" && (clearPrimary || clearSecondary)) {
                enemyBullets_.erase(
                    std::remove_if(
                        enemyBullets_.begin(),
                        enemyBullets_.end(),
                        [enemyDestroyed](const EnemyBullet& bullet) {
                            return bullet.ownerInstanceId() == enemyDestroyed->enemyInstanceId;
                        }
                    ),
                    enemyBullets_.end()
                );
            }
        } else if (const auto* enemyHit = std::get_if<EnemyHitEvent>(&event)) {
            if (enemyHitSparkTexture_) {
                explosions_.emplace_back(
                    enemyHit->position,
                    *enemyHitSparkTexture_,
                    sf::Vector2i{8, 8},
                    2,
                    sf::seconds(0.045f)
                );
            }
        } else if (const auto* itemDestroyed = std::get_if<ItemCarrierDestroyedEvent>(&event)) {
            spawnPowerUp(itemDestroyed->dropId, itemDestroyed->position);
        } else if (const auto* powerUpCollected = std::get_if<PowerUpCollectedEvent>(&event)) {
            if (powerUpCollected->powerUpId == "power_p") {
                player_->collectPowerUpP();
            } else if (powerUpCollected->powerUpId == "power_s") {
                player_->collectPowerUpS();
            }
        } else if (std::holds_alternative<PlayerDestroyedEvent>(event)) {
            playerDestroyed_ = true;
            playerDeathElapsed_ = sf::Time::Zero;
            if (player_ && playerExplosionTexture_) {
                explosions_.emplace_back(player_->position(), *playerExplosionTexture_);
            }
            muzzleFlashTime_ = sf::Time::Zero;
            lockOnCharge_ = sf::Time::Zero;
            lockOnTargetIds_.clear();
            lockOnChargeStarted_ = false;
            playerLasers_.clear();
            homingLasers_.clear();
            enemyLasers_.clear();
        }
    }
}

void PlayState::renderLockOnCharge(sf::RenderTarget& target) const {
    if (!player_ || playerDestroyed_ || fireButtonPressed_ || lockOnCharge_ <= sf::Time::Zero) {
        return;
    }

    const auto& config = lockOnConfigSystem_.config();
    const auto progress = std::clamp(lockOnCharge_.asSeconds() / std::max(0.001f, config.chargeSeconds), 0.f, 1.f);
    const auto origin = player_->position();
    const auto topLeft = sf::Vector2f{
        std::round(origin.x - 18.f),
        std::round(origin.y + 22.f)
    };

    auto border = sf::RectangleShape({36.f, 4.f});
    border.setPosition(topLeft);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(54, 255, 96));
    border.setOutlineThickness(1.f);
    target.draw(border);

    auto fill = sf::RectangleShape({std::round(34.f * progress), 2.f});
    fill.setPosition({topLeft.x + 1.f, topLeft.y + 1.f});
    fill.setFillColor(progress >= 1.f ? sf::Color(180, 255, 190) : sf::Color(54, 255, 96));
    target.draw(fill);
}

void PlayState::renderLockOnField(sf::RenderTarget& target) const {
    if (!player_ || playerDestroyed_ || fireButtonPressed_) {
        return;
    }

    const auto& config = lockOnConfigSystem_.config();
    if (lockOnCharge_.asSeconds() < config.chargeSeconds) {
        return;
    }

    const auto origin = player_->position();
    const auto halfAngle = config.fieldAngleDegrees * 0.5f * Pi / 180.f;
    const auto color = sf::Color(54, 255, 96, 116);
    const auto faint = sf::Color(54, 255, 96, 62);

    auto pointAt = [&](float distance, float normalizedAngle) {
        const auto angle = normalizedAngle * halfAngle;
        return sf::Vector2f{
            origin.x + std::sin(angle) * distance,
            origin.y - std::cos(angle) * distance
        };
    };

    drawLine(target, origin, pointAt(config.fieldRange, -1.f), color);
    drawLine(target, origin, pointAt(config.fieldRange, 1.f), color);
    drawLine(target, origin, pointAt(config.fieldRange, 0.f), faint);
    drawLine(target, origin, pointAt(config.fieldRange, -0.5f), faint);
    drawLine(target, origin, pointAt(config.fieldRange, 0.5f), faint);

    for (auto ring = 1; ring <= 3; ++ring) {
        const auto distance = config.fieldRange * static_cast<float>(ring) / 3.f;
        auto previous = pointAt(distance, -1.f);
        for (auto segment = 1; segment <= 10; ++segment) {
            const auto angleT = -1.f + static_cast<float>(segment) * 0.2f;
            const auto current = pointAt(distance, angleT);
            drawLine(target, previous, current, ring == 3 ? color : faint);
            previous = current;
        }
    }
}

void PlayState::renderLockOnTargets(sf::RenderTarget& target) const {
    if (lockOnTargetIds_.empty()) {
        return;
    }

    const auto color = sf::Color(80, 255, 110);
    for (const auto targetId : lockOnTargetIds_) {
        auto alive = false;
        const auto position = positionForEnemyInstance(targetId, alive);
        if (!alive) {
            continue;
        }

        const auto half = 9.f;
        const auto left = std::round(position.x - half);
        const auto right = std::round(position.x + half);
        const auto top = std::round(position.y - half);
        const auto bottom = std::round(position.y + half);
        const auto corner = 5.f;

        drawLine(target, {left, top}, {left + corner, top}, color);
        drawLine(target, {left, top}, {left, top + corner}, color);
        drawLine(target, {right, top}, {right - corner, top}, color);
        drawLine(target, {right, top}, {right, top + corner}, color);
        drawLine(target, {left, bottom}, {left + corner, bottom}, color);
        drawLine(target, {left, bottom}, {left, bottom - corner}, color);
        drawLine(target, {right, bottom}, {right - corner, bottom}, color);
        drawLine(target, {right, bottom}, {right, bottom - corner}, color);
        drawLine(target, {position.x - 3.f, position.y}, {position.x + 3.f, position.y}, sf::Color(180, 255, 190));
        drawLine(target, {position.x, position.y - 3.f}, {position.x, position.y + 3.f}, sf::Color(180, 255, 190));
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
        if (projectileConfigSystem_.hasTexture(bulletId)) {
            return &assets_.getTexture(bulletId);
        }
    }

    return nullptr;
}

const sf::Texture* PlayState::laserTextureForPattern(const std::string& patternId) const {
    const auto& laserId = bulletPatternSystem_.laserId(patternId);
    if (laserId != "default" && projectileConfigSystem_.hasProjectile(laserId)) {
        if (projectileConfigSystem_.hasTexture(laserId)) {
            return &assets_.getTexture(laserId);
        }
    }

    return nullptr;
}

int PlayState::bulletDamageForPattern(const std::string& patternId) const {
    const auto& bulletId = bulletPatternSystem_.bulletId(patternId);
    if (bulletId != "default" && projectileConfigSystem_.hasProjectile(bulletId)) {
        return projectileConfigSystem_.damageFor(bulletId);
    }

    return 1;
}

int PlayState::laserDamageForPattern(const std::string& patternId) const {
    const auto& laserId = bulletPatternSystem_.laserId(patternId);
    if (laserId != "default" && projectileConfigSystem_.hasProjectile(laserId)) {
        return projectileConfigSystem_.damageFor(laserId);
    }

    return 1;
}

std::string PlayState::bulletVisualTypeForPattern(const std::string& patternId) const {
    const auto& bulletId = bulletPatternSystem_.bulletId(patternId);
    if (bulletId != "default" && projectileConfigSystem_.hasProjectile(bulletId)) {
        return projectileConfigSystem_.visualTypeFor(bulletId);
    }

    return "rect";
}

sf::Vector2f PlayState::bulletVisualSizeForPattern(const std::string& patternId) const {
    const auto& bulletId = bulletPatternSystem_.bulletId(patternId);
    if (bulletId != "default" && projectileConfigSystem_.hasProjectile(bulletId)) {
        return {
            projectileConfigSystem_.visualWidthFor(bulletId),
            projectileConfigSystem_.visualLengthFor(bulletId)
        };
    }

    return {4.f, 4.f};
}

float PlayState::bulletVisualGrowSecondsForPattern(const std::string& patternId) const {
    const auto& bulletId = bulletPatternSystem_.bulletId(patternId);
    if (bulletId != "default" && projectileConfigSystem_.hasProjectile(bulletId)) {
        return projectileConfigSystem_.visualGrowSecondsFor(bulletId);
    }

    return 0.f;
}

bool PlayState::bulletRotateToVelocityForPattern(const std::string& patternId) const {
    const auto& bulletId = bulletPatternSystem_.bulletId(patternId);
    if (bulletId != "default" && projectileConfigSystem_.hasProjectile(bulletId)) {
        return projectileConfigSystem_.rotateToVelocityFor(bulletId);
    }

    return false;
}
