#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <optional>

class EnemyBullet {
public:
    EnemyBullet(sf::Vector2f position, sf::Vector2f velocity, int damage = 1);
    EnemyBullet(sf::Vector2f position, sf::Vector2f velocity, const sf::Texture* texture, int damage = 1);

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;

    bool isAlive(sf::Vector2f logicalSize) const;
    sf::FloatRect hitbox() const;
    int damage() const;

private:
    sf::Vector2f position_;
    sf::Vector2f velocity_;
    sf::Vector2f size_{4.f, 4.f};
    int damage_{1};
    std::optional<sf::Sprite> sprite_;
};
