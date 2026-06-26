#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

class PlayerHomingLaser {
public:
    PlayerHomingLaser(
        sf::Vector2f position,
        int targetInstanceId,
        float sideSign,
        float speed,
        float turnRate,
        float sideExitSeconds,
        float sideExitSpeed,
        float damage,
        sf::Vector2f hitboxSize,
        sf::Vector2f visualSize,
        float coreWidth,
        float glowRadius
    );

    void update(sf::Time deltaTime, sf::Vector2f targetPosition, bool targetAlive);
    void render(sf::RenderTarget& target) const;
    bool isAlive(sf::Vector2f logicalSize) const;
    void destroy();
    sf::FloatRect hitbox() const;
    int targetInstanceId() const;
    float damage() const;

private:
    sf::Vector2f position_;
    sf::Vector2f velocity_;
    sf::Vector2f hitboxSize_;
    sf::Vector2f visualSize_;
    int targetInstanceId_{0};
    float sideSign_{1.f};
    float speed_{0.f};
    float turnRate_{0.f};
    float sideExitSeconds_{0.f};
    float sideExitSpeed_{0.f};
    float damage_{0.f};
    float coreWidth_{1.f};
    float glowRadius_{4.f};
    sf::Time age_{sf::Time::Zero};
    bool alive_{true};
};
