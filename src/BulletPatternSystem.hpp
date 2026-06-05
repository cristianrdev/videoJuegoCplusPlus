#pragma once

#include "EnemyBullet.hpp"

#include <SFML/System/Vector2.hpp>

#include <string>
#include <unordered_map>
#include <vector>

class BulletPatternSystem {
public:
    void loadFromFile(const std::string& path);
    std::vector<EnemyBullet> spawn(const std::string& patternId, sf::Vector2f origin, sf::Vector2f target) const;

private:
    struct Pattern {
        std::string id;
        float fireInterval{1.f};
        float bulletSpeed{60.f};
        bool aimed{false};
        std::vector<float> angleOffsets;
    };

public:
    float fireInterval(const std::string& patternId) const;

private:
    const Pattern& patternFor(const std::string& patternId) const;

    std::unordered_map<std::string, Pattern> patterns_;
};
