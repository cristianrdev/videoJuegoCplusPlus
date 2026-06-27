#pragma once

#include <SFML/System/Vector2.hpp>

#include <string>
#include <unordered_map>
#include <vector>

class WallBossConfigSystem {
public:
    struct CrystalConfig {
        std::string id;
        sf::Vector2f offset{0.f, 0.f};
        std::string patternId{"none"};
        float health{30.f};
        sf::Vector2f hitboxSize{24.f, 24.f};
    };

    struct WallBossConfig {
        std::string id;
        std::string texturePath;
        sf::Vector2f size{240.f, 32.f};
        float speedY{18.f};
        int contactDamage{3};
        float holeWidth{40.f};
        float textureOffsetX{0.f};
        std::vector<CrystalConfig> crystals;
    };

    void loadFromFile(const std::string& path);
    bool hasWallBoss(const std::string& wallBossId) const;
    const WallBossConfig& configFor(const std::string& wallBossId) const;
    const std::string& texturePathFor(const std::string& wallBossId) const;
    std::vector<std::string> wallBossIds() const;

private:
    std::unordered_map<std::string, WallBossConfig> configs_;
};
