#pragma once

#include "Enemy.hpp"
#include "StageDirector.hpp"

class AssetManager;
class EnemyConfigSystem;

class EnemySpawner {
public:
    EnemySpawner(AssetManager& assets, const EnemyConfigSystem& enemyConfigSystem);

    Enemy spawn(const StageDirector::SpawnEvent& spawn) const;

private:
    AssetManager& assets_;
    const EnemyConfigSystem& enemyConfigSystem_;
};
