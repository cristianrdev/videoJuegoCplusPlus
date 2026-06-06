#include "EnemySpawner.hpp"

#include "AssetManager.hpp"
#include "EnemyConfigSystem.hpp"

EnemySpawner::EnemySpawner(AssetManager& assets, const EnemyConfigSystem& enemyConfigSystem)
    : assets_(assets)
    , enemyConfigSystem_(enemyConfigSystem) {
}

Enemy EnemySpawner::spawn(const StageDirector::SpawnEvent& spawn) const {
    return Enemy(
        sf::Vector2f{spawn.x, spawn.y},
        assets_.getTexture(spawn.enemyId),
        spawn.enemyId,
        spawn.patternId,
        spawn.movementId,
        enemyConfigSystem_.healthFor(spawn.enemyId),
        enemyConfigSystem_.blinkEnabledFor(spawn.enemyId),
        enemyConfigSystem_.blinkHealthThresholdFor(spawn.enemyId)
    );
}
