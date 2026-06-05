#pragma once

#include "AssetManager.hpp"
#include "BulletPatternSystem.hpp"
#include "Enemy.hpp"
#include "EnemyBullet.hpp"
#include "LaserNormal.hpp"
#include "Player.hpp"
#include "StageDirector.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <vector>

class Game {
public:
    Game();

    void run();

private:
    enum class PresentationScaleMode {
        Native,
        IntegerFit
    };

    void processEvents();
    void fireLaserNormal();
    void spawnEnemy(const StageDirector::SpawnEvent& spawn);
    void update(sf::Time deltaTime);
    void updateEnemyShooting();
    void updateCollisions();
    void render();
    void renderMuzzleFlash(sf::RenderTarget& target) const;
    void updatePresentationSprite();

    static constexpr unsigned int LogicalWidth = 240;
    static constexpr unsigned int LogicalHeight = 320;

    sf::RenderWindow window_;
    sf::RenderTexture logicalTarget_;
    sf::Sprite presentationSprite_;
    sf::Clock clock_;
    PresentationScaleMode presentationScaleMode_{PresentationScaleMode::IntegerFit};
    std::vector<LaserNormal> playerLasers_;
    std::vector<Enemy> enemies_;
    std::vector<EnemyBullet> enemyBullets_;
    sf::Time fireCooldown_{sf::Time::Zero};
    sf::Time muzzleFlashTime_{sf::Time::Zero};
    const sf::Texture* laserNormalTexture_{nullptr};
    const sf::Texture* muzzleFlashTexture_{nullptr};
    const sf::Texture* enemyDroneTexture_{nullptr};
    const sf::Texture* enemyTurretPodTexture_{nullptr};
    const sf::Texture* enemyInterceptorTexture_{nullptr};
    BulletPatternSystem bulletPatternSystem_;
    StageDirector stageDirector_;

    AssetManager assets_;
    std::unique_ptr<Player> player_;
};
