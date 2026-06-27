#pragma once

#include "AssetManager.hpp"
#include "BackgroundElementConfigSystem.hpp"
#include "BackgroundElement.hpp"
#include "BackgroundElementDirector.hpp"
#include "BulletPatternSystem.hpp"
#include "Enemy.hpp"
#include "EnemyBullet.hpp"
#include "EnemyConfigSystem.hpp"
#include "EnemyLaser.hpp"
#include "EnemySpawner.hpp"
#include "Explosion.hpp"
#include "ItemCarrier.hpp"
#include "ItemConfigSystem.hpp"
#include "LaserNormal.hpp"
#include "MovementPatternSystem.hpp"
#include "Player.hpp"
#include "PlayerConfigSystem.hpp"
#include "PlayerHomingLaser.hpp"
#include "PlayerLockOnConfigSystem.hpp"
#include "ProjectileConfigSystem.hpp"
#include "PowerUpItem.hpp"
#include "StageDirector.hpp"
#include "Starfield.hpp"
#include "WallBoss.hpp"
#include "WallBossConfigSystem.hpp"
#include "CollisionSystem.hpp"
#include "EventQueue.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <string>
#include <vector>

class PlayState {
public:
    PlayState(AssetManager& assets, sf::Vector2f logicalSize);

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target);
    void fireLaserNormal();
    void setFireButtonPressed(bool pressed);
    void onPaused();

    sf::Time stageTime() const;
    bool isPlayerDestroyed() const;
    bool isGameOverVisible() const;
    int playerHealth() const;
    bool isGodModeEnabled() const;
    void toggleGodMode();
    bool isPlayerHitboxVisible() const;
    void togglePlayerHitbox();

private:
    void spawnEnemy(const StageDirector::SpawnEvent& spawn);
    void spawnBackgroundElement(const BackgroundElementDirector::SpawnEvent& spawn);
    void spawnItemCarrier(const ItemSpawnConfig& spawn);
    void spawnPowerUp(const std::string& powerUpId, sf::Vector2f position);
    void spawnExplosion(const std::string& enemyId, sf::Vector2f position);
    void destroyMountedCargoSphere(sf::Vector2f tankPosition);
    void updateLockOn(sf::Time deltaTime);
    void updateHomingLasers(sf::Time deltaTime);
    void resolveHomingLaserCollisions();
    void updateEnemyShooting();
    void updateWallBossShooting();
    void spawnEnemyPattern(Enemy& enemy, const std::string& patternId, sf::Vector2f origin);
    void spawnEnemyPatternForOwner(const std::string& patternId, sf::Vector2f origin, int ownerInstanceId);
    void updateCollisions();
    void resolveWallBossCollisions();
    void processEvents();
    void renderMuzzleFlash(sf::RenderTarget& target) const;
    void renderLockOnCharge(sf::RenderTarget& target) const;
    void renderLockOnField(sf::RenderTarget& target) const;
    void renderLockOnTargets(sf::RenderTarget& target) const;
    void launchLockOnLasers();
    bool enemyInsideLockOnField(const Enemy& enemy) const;
    bool positionInsideLockOnField(sf::Vector2f position) const;
    sf::Vector2f positionForEnemyInstance(int enemyInstanceId, bool& alive) const;
    bool isBulletOwnerAlive(int ownerInstanceId) const;
    void spawnWallBossCrystalExplosion(sf::Vector2f position);
    const sf::Texture* bulletTextureForPattern(const std::string& patternId) const;
    const sf::Texture* laserTextureForPattern(const std::string& patternId) const;
    int bulletDamageForPattern(const std::string& patternId) const;
    int laserDamageForPattern(const std::string& patternId) const;
    std::string bulletVisualTypeForPattern(const std::string& patternId) const;
    sf::Vector2f bulletVisualSizeForPattern(const std::string& patternId) const;
    float bulletVisualGrowSecondsForPattern(const std::string& patternId) const;
    bool bulletRotateToVelocityForPattern(const std::string& patternId) const;

    AssetManager& assets_;
    sf::Vector2f logicalSize_;
    sf::Time stageClock_{sf::Time::Zero};
    std::vector<LaserNormal> playerLasers_;
    std::vector<BackgroundElement> backgroundElements_;
    std::vector<Enemy> enemies_;
    std::vector<EnemyBullet> enemyBullets_;
    std::vector<EnemyLaser> enemyLasers_;
    std::vector<PlayerHomingLaser> homingLasers_;
    std::vector<WallBoss> wallBosses_;
    std::vector<ItemCarrier> itemCarriers_;
    std::vector<PowerUpItem> powerUps_;
    std::vector<Explosion> explosions_;
    std::size_t nextItemSpawnIndex_{0};
    sf::Time fireCooldown_{sf::Time::Zero};
    sf::Time muzzleFlashTime_{sf::Time::Zero};
    const sf::Texture* laserNormalTexture_{nullptr};
    const sf::Texture* muzzleFlashTexture_{nullptr};
    const sf::Texture* explosionDroneTexture_{nullptr};
    const sf::Texture* explosionTurretPodTexture_{nullptr};
    const sf::Texture* explosionInterceptorTexture_{nullptr};
    const sf::Texture* explosionGreenCargoTankTexture_{nullptr};
    const sf::Texture* explosionMechanicalSpikedShellTexture_{nullptr};
    const sf::Texture* explosionArmoredFlailShipTexture_{nullptr};
    const sf::Texture* playerExplosionTexture_{nullptr};
    const sf::Texture* enemyHitSparkTexture_{nullptr};
    const sf::Texture* floatingRedRocksTexture_{nullptr};
    const sf::Texture* earthLikePlanetTexture_{nullptr};
    BulletPatternSystem bulletPatternSystem_;
    EnemyConfigSystem enemyConfigSystem_;
    EnemySpawner enemySpawner_;
    ProjectileConfigSystem projectileConfigSystem_;
    ItemConfigSystem itemConfigSystem_;
    PlayerConfigSystem playerConfigSystem_;
    PlayerLockOnConfigSystem lockOnConfigSystem_;
    WallBossConfigSystem wallBossConfigSystem_;
    MovementPatternSystem movementPatternSystem_;
    CollisionSystem collisionSystem_;
    EventQueue eventQueue_;
    StageDirector stageDirector_;
    BackgroundElementDirector backgroundElementDirector_;
    BackgroundElementConfigSystem backgroundElementConfigSystem_;
    Starfield starfield_;
    std::unique_ptr<Player> player_;
    sf::Time playerDeathElapsed_{sf::Time::Zero};
    sf::Time lockOnCharge_{sf::Time::Zero};
    std::vector<int> lockOnTargetIds_;
    bool playerDestroyed_{false};
    bool fireButtonPressed_{false};
    bool wasFireButtonPressed_{false};
    bool lockOnChargeStarted_{false};
};
