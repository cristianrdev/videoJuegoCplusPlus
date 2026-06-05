#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

class LaserNormal {
public:
    static constexpr int Damage = 1;

    LaserNormal(sf::Vector2f position, const sf::Texture& texture);

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;

    bool isAlive() const;
    sf::FloatRect hitbox() const;

private:
    sf::Vector2f position_;
    sf::Vector2f size_{4.f, 12.f};
    float speed_{260.f};
    sf::Sprite sprite_;
};
