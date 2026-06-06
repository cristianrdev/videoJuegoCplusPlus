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
#include "StageDirector.hpp"
#include "Starfield.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <string>
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
    void togglePause();
    void fireLaserNormal();
    void spawnEnemy(const StageDirector::SpawnEvent& spawn);
    void spawnBackgroundElement(const BackgroundElementDirector::SpawnEvent& spawn);
    void spawnExplosion(const Enemy& enemy);
    void update(sf::Time deltaTime);
    void updateEnemyShooting();
    void updateCollisions();
    void render();
    void renderMuzzleFlash(sf::RenderTarget& target) const;
    void renderPauseOverlay();
    void renderDebugHud();
    void updatePresentationSprite();
    const sf::Texture* bulletTextureForPattern(const std::string& patternId) const;
    const sf::Texture* laserTextureForPattern(const std::string& patternId) const;

    static constexpr unsigned int LogicalWidth = 240;
    static constexpr unsigned int LogicalHeight = 320;

    sf::RenderWindow window_;
    sf::RenderTexture logicalTarget_;
    sf::Sprite presentationSprite_;
    sf::Clock clock_;
    sf::Font debugFont_;
    sf::Text debugText_;
    PresentationScaleMode presentationScaleMode_{PresentationScaleMode::IntegerFit};
    bool paused_{false};
    sf::Time stageClock_{sf::Time::Zero};
    float smoothedFps_{0.f};
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
    const sf::Texture* enemyOrbPurpleTexture_{nullptr};
    const sf::Texture* enemyRobotFishLaserTexture_{nullptr};
    const sf::Texture* floatingRedRocksTexture_{nullptr};
    BulletPatternSystem bulletPatternSystem_;
    EnemyConfigSystem enemyConfigSystem_;
    MovementPatternSystem movementPatternSystem_;
    StageDirector stageDirector_;
    BackgroundElementDirector backgroundElementDirector_;
    Starfield starfield_;

    AssetManager assets_;
    std::unique_ptr<Player> player_;
};
