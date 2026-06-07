#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>

class BackgroundElement {
public:
    BackgroundElement(
        sf::Vector2f position,
        float speedY,
        const sf::Texture& texture,
        int tileIndex,
        sf::Vector2i tileSize,
        std::string hitboxShape,
        sf::Vector2f hitboxOffset,
        sf::Vector2f hitboxSize,
        int contactDamage = 0
    );

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target, bool showHitbox = false) const;

    bool isAlive(float logicalHeight) const;
    sf::FloatRect hitbox() const;
    bool intersects(sf::FloatRect rect) const;
    int contactDamage() const;

private:
    void renderHitbox(sf::RenderTarget& target) const;

    sf::Vector2f position_;
    float speedY_{0.f};
    sf::Vector2i tileSize_;
    std::string hitboxShape_{"square"};
    sf::Vector2f hitboxOffset_;
    sf::Vector2f hitboxSize_;
    int contactDamage_{0};
    sf::Sprite sprite_;
};
