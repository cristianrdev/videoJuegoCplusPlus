#include "CollisionSystem.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace {
bool intersects(sf::FloatRect left, sf::FloatRect right) {
    return left.position.x < right.position.x + right.size.x &&
        left.position.x + left.size.x > right.position.x &&
        left.position.y < right.position.y + right.size.y &&
        left.position.y + left.size.y > right.position.y;
}

bool damagePlayer(Player& player, int damage, EventQueue& eventQueue) {
    if (!player.isAlive()) {
        return false;
    }

    if (!player.takeDamage(damage)) {
        return false;
    }

    eventQueue.publish(PlayerHitEvent{damage, player.health()});
    if (!player.isAlive()) {
        eventQueue.publish(PlayerDestroyedEvent{});
    }

    return true;
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
            damagePlayer(player, bulletIt->damage(), eventQueue);
            bulletIt = enemyBullets.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }

    for (auto& laser : enemyLasers) {
        if (laser.canHitPlayer() && intersects(laser.hitbox(), player.hitbox())) {
            if (damagePlayer(player, laser.damage(), eventQueue)) {
                laser.markPlayerHit();
            }
        }
    }

    for (auto& enemy : enemies) {
        if (!enemy.isAlive() || enemy.contactDamage() <= 0) {
            continue;
        }

        if (intersects(enemy.hitbox(), player.hitbox())) {
            damagePlayer(player, enemy.contactDamage(), eventQueue);
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
