#pragma once

#include <string>
#include <unordered_map>

class EnemyConfigSystem {
public:
    void loadFromFile(const std::string& path);
    int healthFor(const std::string& enemyId) const;

private:
    struct EnemyConfig {
        std::string id;
        int health{3};
    };

    std::unordered_map<std::string, EnemyConfig> configs_;
};
