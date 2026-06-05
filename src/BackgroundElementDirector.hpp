#pragma once

#include <SFML/System/Time.hpp>

#include <string>
#include <vector>

class BackgroundElementDirector {
public:
    struct SpawnEvent {
        float time{0.f};
        std::string tilesetId;
        int tileIndex{0};
        float x{0.f};
        float y{0.f};
        float speedY{20.f};
    };

    void loadFromFile(const std::string& path);
    std::vector<SpawnEvent> update(sf::Time deltaTime);
    void reset();

private:
    std::vector<SpawnEvent> spawns_;
    sf::Time elapsed_{sf::Time::Zero};
    std::size_t nextSpawnIndex_{0};
};
