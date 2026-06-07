#include "EnemySpawner.hpp"

#include "AssetManager.hpp"
#include "EnemyConfigSystem.hpp"

#include <array>
#include <random>

EnemySpawner::EnemySpawner(AssetManager& assets, const EnemyConfigSystem& enemyConfigSystem)
    : assets_(assets)
    , enemyConfigSystem_(enemyConfigSystem) {
}

Enemy EnemySpawner::spawn(const StageDirector::SpawnEvent& spawn) const {
    auto movementId = spawn.movementId;
    if (movementId == "mech_spider_random_zigzag") {
        static auto randomEngine = std::mt19937{std::random_device{}()};
        static constexpr auto variants = std::array{
            "mech_spider_zigzag_left_left",
            "mech_spider_zigzag_left_right",
            "mech_spider_zigzag_right_left",
            "mech_spider_zigzag_right_right"
        };
        auto distribution = std::uniform_int_distribution<std::size_t>{0, variants.size() - 1};
        movementId = variants[distribution(randomEngine)];
    }

    return Enemy(
        sf::Vector2f{spawn.x, spawn.y},
        assets_.getTexture(spawn.enemyId),
        spawn.enemyId,
        spawn.patternId,
        movementId,
        enemyConfigSystem_.healthFor(spawn.enemyId),
        enemyConfigSystem_.contactDamageFor(spawn.enemyId),
        enemyConfigSystem_.hitboxSizeFor(spawn.enemyId),
        enemyConfigSystem_.hitboxOffsetFor(spawn.enemyId),
        enemyConfigSystem_.blinkEnabledFor(spawn.enemyId),
        enemyConfigSystem_.blinkHealthThresholdFor(spawn.enemyId)
    );
}
