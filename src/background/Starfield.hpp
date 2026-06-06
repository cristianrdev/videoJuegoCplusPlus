#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <random>
#include <vector>

class Starfield {
public:
    explicit Starfield(sf::Vector2f logicalSize);

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;

private:
    struct Star {
        sf::Vector2f position;
        float speed{0.f};
        float phase{0.f};
        float twinkleSpeed{0.f};
        bool twinkles{false};
        int size{1};
        sf::Color color;
    };

    Star createStar(bool randomizeY);
    void resetStar(Star& star);

    sf::Vector2f logicalSize_;
    std::vector<Star> stars_;
    std::mt19937 rng_;
    sf::Time elapsed_{sf::Time::Zero};
};
