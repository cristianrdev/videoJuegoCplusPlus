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
        float dropY{0.f};
        float squareHalfWidth{40.f};
        float squareHalfHeight{30.f};
        float squarePeriod{4.f};
        float squareCenterOffsetY{-70.f};
        float screenWidth{240.f};
        float screenHeight{320.f};
        float marginX{25.f};
        float marginTop{50.f};
        float marginBottom{50.f};
        std::vector<float> pointTimes;
        std::vector<float> pointX;
        std::vector<float> pointY;
    };

    void loadFromFile(const std::string& path);
    sf::Vector2f positionFor(const std::string& patternId, sf::Vector2f startPosition, sf::Time elapsed) const;
    sf::Vector2f positionFor(
        const std::string& patternId,
        sf::Vector2f startPosition,
        sf::Time elapsed,
        sf::Vector2f targetPosition
    ) const;

private:
    const Pattern& patternFor(const std::string& patternId) const;

    std::unordered_map<std::string, Pattern> patterns_;
};
