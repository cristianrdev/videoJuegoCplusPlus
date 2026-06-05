#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>

class Enemy {
public:
    Enemy(sf::Vector2f position, const sf::Texture& texture, std::string patternId);

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;
    void takeDamage(int damage);

    bool isAlive() const;
    bool shouldFire() const;
    void resetFireTimer(float intervalSeconds);
    sf::Vector2f bulletSpawnPosition() const;
    sf::Vector2f position() const;
    sf::FloatRect hitbox() const;
    const std::string& patternId() const;

private:
    sf::Vector2f position_;
    sf::Vector2f size_{24.f, 24.f};
    float driftSpeed_{14.f};
    int health_{3};
    sf::Time fireTimer_{sf::Time::Zero};
    std::string patternId_;
    sf::Sprite sprite_;
};
