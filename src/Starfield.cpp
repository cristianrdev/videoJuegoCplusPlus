#include "Starfield.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <algorithm>
#include <array>
#include <cmath>

namespace {
float randomFloat(std::mt19937& rng, float min, float max) {
    auto distribution = std::uniform_real_distribution<float>{min, max};
    return distribution(rng);
}

int randomInt(std::mt19937& rng, int min, int max) {
    auto distribution = std::uniform_int_distribution<int>{min, max};
    return distribution(rng);
}
}

Starfield::Starfield(sf::Vector2f logicalSize)
    : logicalSize_(logicalSize)
    , rng_(0x240320u) {
    stars_.reserve(90);
    for (auto i = 0; i < 90; ++i) {
        stars_.push_back(createStar(true));
    }
}

void Starfield::update(sf::Time deltaTime) {
    elapsed_ += deltaTime;

    for (auto& star : stars_) {
        star.position.y += star.speed * deltaTime.asSeconds();
        if (star.position.y >= logicalSize_.y + 2.f) {
            resetStar(star);
        }
    }
}

void Starfield::render(sf::RenderTarget& target) const {
    auto pixel = sf::RectangleShape{};

    for (const auto& star : stars_) {
        auto alpha = static_cast<float>(star.color.a);
        if (star.twinkles) {
            const auto pulse = 0.65f + 0.35f * std::sin(elapsed_.asSeconds() * star.twinkleSpeed + star.phase);
            alpha *= pulse;
        }

        auto color = star.color;
        color.a = static_cast<std::uint8_t>(std::clamp(alpha, 30.f, 255.f));

        pixel.setSize({
            static_cast<float>(star.size),
            static_cast<float>(star.size)
        });
        pixel.setFillColor(color);
        pixel.setPosition({
            std::round(star.position.x),
            std::round(star.position.y)
        });
        target.draw(pixel);
    }
}

Starfield::Star Starfield::createStar(bool randomizeY) {
    static const auto palette = std::array<sf::Color, 6>{
        sf::Color(80, 110, 150, 130),
        sf::Color(120, 150, 190, 155),
        sf::Color(170, 205, 230, 180),
        sf::Color(210, 235, 255, 220),
        sf::Color(150, 210, 255, 190),
        sf::Color(235, 245, 255, 240)
    };

    auto star = Star{};
    star.position = {
        randomFloat(rng_, 0.f, logicalSize_.x - 1.f),
        randomizeY ? randomFloat(rng_, 0.f, logicalSize_.y - 1.f) : randomFloat(rng_, -16.f, -1.f)
    };

    const auto layer = randomInt(rng_, 0, 3);
    star.speed = 12.f + static_cast<float>(layer) * randomFloat(rng_, 16.f, 26.f);
    star.size = layer == 3 && randomInt(rng_, 0, 4) == 0 ? 2 : 1;
    star.color = palette[static_cast<std::size_t>(randomInt(rng_, 0, static_cast<int>(palette.size() - 1)))];
    star.twinkles = randomInt(rng_, 0, 99) < 38;
    star.twinkleSpeed = randomFloat(rng_, 2.2f, 7.0f);
    star.phase = randomFloat(rng_, 0.f, 6.28318f);
    return star;
}

void Starfield::resetStar(Star& star) {
    star = createStar(false);
}
