#pragma once

#include "Enemy.hpp"
#include "EnemyBullet.hpp"
#include "EnemyLaser.hpp"
#include "EventQueue.hpp"
#include "ItemCarrier.hpp"
#include "LaserNormal.hpp"
#include "Player.hpp"
#include "PowerUpItem.hpp"

#include <vector>

class CollisionSystem {
public:
    void resolve(
        std::vector<LaserNormal>& playerLasers,
        std::vector<Enemy>& enemies,
        std::vector<EnemyBullet>& enemyBullets,
        std::vector<EnemyLaser>& enemyLasers,
        Player& player,
        EventQueue& eventQueue
    ) const;

    void resolveItems(
        std::vector<LaserNormal>& playerLasers,
        std::vector<ItemCarrier>& itemCarriers,
        std::vector<PowerUpItem>& powerUps,
        Player& player,
        sf::Vector2f logicalSize,
        EventQueue& eventQueue
    ) const;
};
