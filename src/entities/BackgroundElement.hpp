#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

class BackgroundElement {
public:
    BackgroundElement(
        sf::Vector2f position,
        float speedY,
        const sf::Texture& texture,
        int tileIndex,
        sf::Vector2i tileSize,
        int contactDamage = 0
    );

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;

    bool isAlive(float logicalHeight) const;
    sf::FloatRect hitbox() const;
    int contactDamage() const;

private:
    sf::Vector2f position_;
    float speedY_{0.f};
    sf::Vector2i tileSize_;
    int contactDamage_{0};
    sf::Sprite sprite_;
};
