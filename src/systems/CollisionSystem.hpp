#pragma once

#include "Enemy.hpp"
#include "EnemyBullet.hpp"
#include "EnemyLaser.hpp"
#include "LaserNormal.hpp"
#include "Player.hpp"

#include <SFML/System/Vector2.hpp>

#include <string>
#include <vector>

class CollisionSystem {
public:
    struct DestroyedEnemy {
        std::string enemyId;
        sf::Vector2f position;
    };

    struct Result {
        std::vector<DestroyedEnemy> destroyedEnemies;
    };

    Result resolve(
        std::vector<LaserNormal>& playerLasers,
        std::vector<Enemy>& enemies,
        std::vector<EnemyBullet>& enemyBullets,
        std::vector<EnemyLaser>& enemyLasers,
        Player& player
    ) const;
};
