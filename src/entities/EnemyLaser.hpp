#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

class EnemyLaser {
public:
    EnemyLaser(sf::Vector2f origin, float length, sf::Time duration, const sf::Texture& texture);

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;

    bool isAlive() const;
    bool canHitPlayer() const;
    void markPlayerHit();
    sf::FloatRect hitbox() const;

private:
    sf::Vector2f origin_;
    sf::Vector2f size_;
    sf::Time remaining_;
    bool hasHitPlayer_{false};
    sf::Sprite sprite_;
};
