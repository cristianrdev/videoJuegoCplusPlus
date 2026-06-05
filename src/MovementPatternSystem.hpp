#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>
#include <unordered_map>
#include <vector>

class MovementPatternSystem {
public:
    struct Pattern {
        std::string id;
        std::string type{"linear"};
        float velocityX{0.f};
        float velocityY{0.f};
        float amplitudeX{0.f};
        float frequency{1.f};
        float phase{0.f};
        std::vector<float> pointTimes;
        std::vector<float> pointX;
        std::vector<float> pointY;
    };

    void loadFromFile(const std::string& path);
    sf::Vector2f positionFor(const std::string& patternId, sf::Vector2f startPosition, sf::Time elapsed) const;

private:
    const Pattern& patternFor(const std::string& patternId) const;

    std::unordered_map<std::string, Pattern> patterns_;
};
