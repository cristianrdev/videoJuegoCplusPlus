#pragma once

#include <SFML/System/Vector2.hpp>

#include <string>
#include <unordered_map>

struct BackgroundElementConfig {
    sf::Vector2f hitboxOffset{0.f, 0.f};
    sf::Vector2f hitboxSize{100.f, 100.f};
};

class BackgroundElementConfigSystem {
public:
    void loadFromFile(const std::string& path);
    BackgroundElementConfig configFor(const std::string& tilesetId, int tileIndex) const;

private:
    static std::string keyFor(const std::string& tilesetId, int tileIndex);

    std::unordered_map<std::string, BackgroundElementConfig> configs_;
};
