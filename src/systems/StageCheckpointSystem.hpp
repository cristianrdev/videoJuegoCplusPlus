#pragma once

#include <SFML/System/Time.hpp>

#include <string>
#include <vector>

class StageCheckpointSystem {
public:
    struct Checkpoint {
        std::string id;
        sf::Time time{sf::Time::Zero};
    };

    void loadFromFile(const std::string& path);
    int activeCheckpointIndex(sf::Time stageTime) const;
    sf::Time activeCheckpointTime(sf::Time stageTime) const;

private:
    std::vector<Checkpoint> checkpoints_;
};
