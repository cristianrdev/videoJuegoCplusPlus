#pragma once

#include <SFML/System/Vector2.hpp>

#include <string>
#include <unordered_map>
#include <vector>

class EnemyConfigSystem {
public:
    void loadFromFile(const std::string& path);
    int healthFor(const std::string& enemyId) const;
    int contactDamageFor(const std::string& enemyId) const;
    sf::Vector2f hitboxSizeFor(const std::string& enemyId) const;
    sf::Vector2f hitboxOffsetFor(const std::string& enemyId) const;
    bool blinkEnabledFor(const std::string& enemyId) const;
    int blinkHealthThresholdFor(const std::string& enemyId) const;
    const std::string& texturePathFor(const std::string& enemyId) const;
    std::vector<std::string> enemyIds() const;

private:
    struct EnemyConfig {
        std::string id;
        std::string texturePath;
        int health{3};
        int contactDamage{0};
        sf::Vector2f hitboxSize{0.f, 0.f};
        sf::Vector2f hitboxOffset{0.f, 0.f};
        bool blinkEnabled{false};
        int blinkHealthThreshold{0};
    };

    std::unordered_map<std::string, EnemyConfig> configs_;
};
