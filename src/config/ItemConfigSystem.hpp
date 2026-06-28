#pragma once

#include <SFML/System/Time.hpp>

#include <string>
#include <unordered_map>
#include <vector>

struct ItemCarrierConfig {
    std::string id;
    std::string texturePath;
    std::string dropId;
    int health{5};
    float speedY{24.f};
    float hitboxWidth{15.f};
    float hitboxHeight{15.f};
};

struct PowerUpConfig {
    std::string id;
    std::string texturePath;
    float lifetimeSeconds{30.f};
    float flickerSeconds{10.f};
    float amplitude{28.f};
    float frequency{1.4f};
    float speedY{8.f};
    float hitboxWidth{13.f};
    float hitboxHeight{13.f};
    std::string transformTo{"none"};
    float transformIntervalSeconds{0.f};
};

struct ItemSpawnConfig {
    sf::Time time{sf::Time::Zero};
    std::string itemId;
    float x{0.f};
    float y{0.f};
};

class ItemConfigSystem {
public:
    void loadFromFile(const std::string& path);
    void loadStageSpawnsFromFile(const std::string& path);

    const ItemCarrierConfig& carrierConfigFor(const std::string& itemId) const;
    const PowerUpConfig& powerUpConfigFor(const std::string& powerUpId) const;
    const std::vector<ItemSpawnConfig>& spawns() const;
    std::vector<std::string> textureIds() const;
    const std::string& texturePathFor(const std::string& itemId) const;

private:
    std::unordered_map<std::string, ItemCarrierConfig> carriers_;
    std::unordered_map<std::string, PowerUpConfig> powerUps_;
    std::vector<ItemSpawnConfig> spawns_;
};
