#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>

class Enemy {
public:
    Enemy(
        sf::Vector2f position,
        const sf::Texture& texture,
        std::string enemyId,
        std::string patternId,
        std::string movementId,
        int health,
        int contactDamage,
        sf::Vector2f configuredHitboxSize,
        sf::Vector2f configuredHitboxOffset,
        bool blinkEnabled,
        int blinkHealthThreshold
    );

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target, bool showHitbox = false) const;
    void takeDamage(int damage);
    void setPosition(sf::Vector2f position);

    bool isAlive() const;
    bool shouldFire() const;
    int contactDamage() const;
    void resetFireTimer(float intervalSeconds);
    void startFiringVisual(sf::Time duration);
    bool shouldRenderNegative() const;
    sf::Vector2f bulletSpawnPosition() const;
    sf::Vector2f position() const;
    sf::Vector2f startPosition() const;
    sf::Time elapsed() const;
    sf::FloatRect hitbox() const;
    const std::string& enemyId() const;
    const std::string& patternId() const;
    const std::string& movementId() const;

private:
    void renderHitbox(sf::RenderTarget& target) const;

    sf::Vector2f startPosition_;
    sf::Vector2f position_;
    sf::Vector2f size_{24.f, 24.f};
    sf::Vector2f hitboxOffset_{0.f, 0.f};
    sf::Vector2f visualSize_{24.f, 24.f};
    int health_{3};
    int contactDamage_{0};
    sf::Time elapsed_{sf::Time::Zero};
    sf::Time fireTimer_{sf::Time::Zero};
    sf::Time firingVisualTime_{sf::Time::Zero};
    bool blinkEnabled_{false};
    int blinkHealthThreshold_{0};
    std::string enemyId_;
    std::string patternId_;
    std::string movementId_;
    sf::Sprite sprite_;
};
