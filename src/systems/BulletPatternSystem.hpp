#pragma once

#include "EnemyBullet.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>
#include <unordered_map>
#include <vector>

class BulletPatternSystem {
public:
    void loadFromFile(const std::string& path);
    std::vector<EnemyBullet> spawn(
        const std::string& patternId,
        sf::Vector2f origin,
        sf::Vector2f target,
        const sf::Texture* bulletTexture = nullptr,
        int bulletDamage = 1,
        const std::string& visualType = "rect",
        sf::Vector2f visualSize = {4.f, 4.f},
        float visualGrowSeconds = 0.f
    );

private:
    struct Pattern {
        std::string id;
        std::string bulletId{"default"};
        std::string laserId{"default"};
        std::string type{"spread"};
        float fireInterval{1.f};
        float laserDuration{0.35f};
        float bulletSpeed{60.f};
        bool aimed{false};
        std::vector<float> angleOffsets;
        int rings{1};
        int bulletsPerRing{1};
        int streams{1};
        float speedStart{60.f};
        float speedStep{0.f};
        float angleStep{0.f};
        float rotationPerShot{0.f};
        int shotCounter{0};
    };

public:
    float fireInterval(const std::string& patternId) const;
    const std::string& bulletId(const std::string& patternId) const;
    const std::string& laserId(const std::string& patternId) const;
    const std::string& patternType(const std::string& patternId) const;
    float laserDuration(const std::string& patternId) const;

private:
    const Pattern& patternFor(const std::string& patternId) const;

    std::unordered_map<std::string, Pattern> patterns_;
};
