#pragma once

#include "AssetManager.hpp"
#include "BackgroundElement.hpp"
#include "BackgroundElementDirector.hpp"
#include "BulletPatternSystem.hpp"
#include "Enemy.hpp"
#include "EnemyBullet.hpp"
#include "EnemyConfigSystem.hpp"
#include "EnemyLaser.hpp"
#include "Explosion.hpp"
#include "LaserNormal.hpp"
#include "MovementPatternSystem.hpp"
#include "Player.hpp"
#include "PlayerConfigSystem.hpp"
#include "ProjectileConfigSystem.hpp"
#include "StageDirector.hpp"
#include "Starfield.hpp"

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
    void onPaused();

    sf::Time stageTime() const;

private:
    void spawnEnemy(const StageDirector::SpawnEvent& spawn);
    void spawnBackgroundElement(const BackgroundElementDirector::SpawnEvent& spawn);
    void spawnExplosion(const Enemy& enemy);
    void updateEnemyShooting();
    void updateCollisions();
    void renderMuzzleFlash(sf::RenderTarget& target) const;
    const sf::Texture* bulletTextureForPattern(const std::string& patternId) const;
    const sf::Texture* laserTextureForPattern(const std::string& patternId) const;

    AssetManager& assets_;
    sf::Vector2f logicalSize_;
    sf::Time stageClock_{sf::Time::Zero};
    std::vector<LaserNormal> playerLasers_;
    std::vector<BackgroundElement> backgroundElements_;
    std::vector<Enemy> enemies_;
    std::vector<EnemyBullet> enemyBullets_;
    std::vector<EnemyLaser> enemyLasers_;
    std::vector<Explosion> explosions_;
    sf::Time fireCooldown_{sf::Time::Zero};
    sf::Time muzzleFlashTime_{sf::Time::Zero};
    const sf::Texture* laserNormalTexture_{nullptr};
    const sf::Texture* muzzleFlashTexture_{nullptr};
    const sf::Texture* explosionDroneTexture_{nullptr};
    const sf::Texture* explosionTurretPodTexture_{nullptr};
    const sf::Texture* explosionInterceptorTexture_{nullptr};
    const sf::Texture* floatingRedRocksTexture_{nullptr};
    BulletPatternSystem bulletPatternSystem_;
    EnemyConfigSystem enemyConfigSystem_;
    ProjectileConfigSystem projectileConfigSystem_;
    PlayerConfigSystem playerConfigSystem_;
    MovementPatternSystem movementPatternSystem_;
    StageDirector stageDirector_;
    BackgroundElementDirector backgroundElementDirector_;
    Starfield starfield_;
    std::unique_ptr<Player> player_;
};
