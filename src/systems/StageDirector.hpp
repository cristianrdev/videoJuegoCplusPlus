#pragma once

#include <SFML/System/Time.hpp>

#include <string>
#include <vector>

class StageDirector {
public:
    struct SpawnEvent {
        float time{0.f};
        std::string enemyId;
        float x{0.f};
        float y{0.f};
        std::string patternId;
        std::string movementId;
    };

    void loadFromFile(const std::string& path);
    std::vector<SpawnEvent> update(sf::Time deltaTime);
    void seekTo(sf::Time elapsed);
    void reset();

private:
    std::vector<SpawnEvent> spawns_;
    sf::Time elapsed_{sf::Time::Zero};
    std::size_t nextSpawnIndex_{0};
};
