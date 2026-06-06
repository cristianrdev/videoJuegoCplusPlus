#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class EnemyConfigSystem {
public:
    void loadFromFile(const std::string& path);
    int healthFor(const std::string& enemyId) const;
    const std::string& texturePathFor(const std::string& enemyId) const;
    std::vector<std::string> enemyIds() const;

private:
    struct EnemyConfig {
        std::string id;
        std::string texturePath;
        int health{3};
    };

    std::unordered_map<std::string, EnemyConfig> configs_;
};
