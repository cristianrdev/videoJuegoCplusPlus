#include "CollisionSystem.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace {
bool intersects(sf::FloatRect left, sf::FloatRect right) {
    return left.position.x < right.position.x + right.size.x &&
        left.position.x + left.size.x > right.position.x &&
        left.position.y < right.position.y + right.size.y &&
        left.position.y + left.size.y > right.position.y;
}
}

void CollisionSystem::resolve(
    std::vector<LaserNormal>& playerLasers,
    std::vector<Enemy>& enemies,
    std::vector<EnemyBullet>& enemyBullets,
    std::vector<EnemyLaser>& enemyLasers,
    Player& player,
    EventQueue& eventQueue
) const {
    for (auto laserIt = playerLasers.begin(); laserIt != playerLasers.end();) {
        auto hitEnemy = false;

        for (auto& enemy : enemies) {
            if (!enemy.isAlive()) {
                continue;
            }

            if (intersects(laserIt->hitbox(), enemy.hitbox())) {
                enemy.takeDamage(laserIt->damage());
                if (!enemy.isAlive()) {
                    eventQueue.publish(EnemyDestroyedEvent{enemy.enemyId(), enemy.position()});
                }
                hitEnemy = true;
                break;
            }
        }

        if (hitEnemy) {
            laserIt = playerLasers.erase(laserIt);
        } else {
            ++laserIt;
        }
    }

    for (auto bulletIt = enemyBullets.begin(); bulletIt != enemyBullets.end();) {
        if (intersects(bulletIt->hitbox(), player.hitbox())) {
            player.takeDamage(1);
            eventQueue.publish(PlayerHitEvent{1, player.health()});
            bulletIt = enemyBullets.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }

    for (auto& laser : enemyLasers) {
        if (laser.canHitPlayer() && intersects(laser.hitbox(), player.hitbox())) {
            player.takeDamage(1);
            eventQueue.publish(PlayerHitEvent{1, player.health()});
            laser.markPlayerHit();
        }
    }

    for (auto& enemy : enemies) {
        if (!enemy.isAlive() || enemy.contactDamage() <= 0) {
            continue;
        }

        if (intersects(enemy.hitbox(), player.hitbox())) {
            player.takeDamage(enemy.contactDamage());
            eventQueue.publish(PlayerHitEvent{enemy.contactDamage(), player.health()});
            enemy.takeDamage(9999);
            eventQueue.publish(EnemyDestroyedEvent{enemy.enemyId(), enemy.position()});
        }
    }
}

void CollisionSystem::resolveItems(
    std::vector<LaserNormal>& playerLasers,
    std::vector<ItemCarrier>& itemCarriers,
    std::vector<PowerUpItem>& powerUps,
    Player& player,
    sf::Vector2f logicalSize,
    EventQueue& eventQueue
) const {
    for (auto laserIt = playerLasers.begin(); laserIt != playerLasers.end();) {
        auto hitCarrier = false;

        for (auto& carrier : itemCarriers) {
            if (!carrier.isAlive(logicalSize)) {
                continue;
            }

            if (intersects(laserIt->hitbox(), carrier.hitbox())) {
                carrier.takeDamage(laserIt->damage());
                if (carrier.isDestroyed()) {
                    eventQueue.publish(ItemCarrierDestroyedEvent{carrier.dropId(), carrier.position()});
                }
                hitCarrier = true;
                break;
            }
        }

        if (hitCarrier) {
            laserIt = playerLasers.erase(laserIt);
        } else {
            ++laserIt;
        }
    }

    for (auto powerUpIt = powerUps.begin(); powerUpIt != powerUps.end();) {
        if (intersects(powerUpIt->hitbox(), player.hitbox())) {
            eventQueue.publish(PowerUpCollectedEvent{powerUpIt->powerUpId()});
            powerUpIt = powerUps.erase(powerUpIt);
        } else {
            ++powerUpIt;
        }
    }
}
