#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <optional>
#include <string>

class EnemyBullet {
public:
    EnemyBullet(sf::Vector2f position, sf::Vector2f velocity, int damage = 1);
    EnemyBullet(sf::Vector2f position, sf::Vector2f velocity, const sf::Texture* texture, int damage = 1);
    EnemyBullet(
        sf::Vector2f position,
        sf::Vector2f velocity,
        const sf::Texture* texture,
        int damage,
        std::string visualType,
        sf::Vector2f visualSize,
        float visualGrowSeconds = 0.f
    );

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;

    bool isAlive(sf::Vector2f logicalSize) const;
    sf::FloatRect hitbox() const;
    bool isPixelLine() const;
    void setPosition(sf::Vector2f position);
    int damage() const;

private:
    sf::Vector2f position_;
    sf::Vector2f velocity_;
    sf::Vector2f size_{4.f, 4.f};
    std::string visualType_{"rect"};
    sf::Time age_{sf::Time::Zero};
    sf::Time growDuration_{sf::Time::Zero};
    int damage_{1};
    std::optional<sf::Sprite> sprite_;
};
