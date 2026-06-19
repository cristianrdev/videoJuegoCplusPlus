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
        float visualGrowSeconds = 0.f,
        int ownerInstanceId = 0,
        bool rotateToVelocity = false,
        float maxLifetimeSeconds = 0.f,
        float flickerBeforeDeathSeconds = 0.f
    );
    EnemyBullet(
        sf::Vector2f position,
        sf::Vector2f velocity,
        const sf::Texture* texture,
        int damage,
        std::string visualType,
        sf::Vector2f visualSize,
        float visualGrowSeconds,
        int ownerInstanceId,
        bool rotateToVelocity,
        float maxLifetimeSeconds,
        float flickerBeforeDeathSeconds,
        sf::Vector2f polarOrigin,
        sf::Vector2f polarOriginVelocity,
        float initialRadius,
        float targetRadius,
        float radiusOpenSeconds,
        float polarAngleRadians,
        float radialSpeed,
        float angularVelocityRadians
    );
    static EnemyBullet tetheredFlail(
        sf::Vector2f origin,
        const sf::Texture* texture,
        int damage,
        sf::Vector2f visualSize,
        int ownerInstanceId,
        float orbitRadius,
        float throwRadius,
        float orbitSeconds,
        float extendSeconds,
        float holdSeconds,
        float retractSeconds,
        float initialAngleRadians,
        float angularVelocityRadians
    );

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;

    bool isAlive(sf::Vector2f logicalSize) const;
    sf::FloatRect hitbox() const;
    bool isPixelLine() const;
    bool followsOwnerAnchor() const;
    bool persistsOnHit() const;
    void setPosition(sf::Vector2f position);
    void setTetherOrigin(sf::Vector2f origin);
    int ownerInstanceId() const;
    int damage() const;

private:
    sf::Vector2f position_;
    sf::Vector2f velocity_;
    sf::Vector2f size_{4.f, 4.f};
    std::string visualType_{"rect"};
    sf::Time age_{sf::Time::Zero};
    sf::Time growDuration_{sf::Time::Zero};
    sf::Time maxLifetime_{sf::Time::Zero};
    sf::Time flickerBeforeDeath_{sf::Time::Zero};
    int ownerInstanceId_{0};
    bool rotateToVelocity_{false};
    bool usesPolarMotion_{false};
    bool usesTetheredFlail_{false};
    sf::Vector2f polarOrigin_{};
    sf::Vector2f polarOriginVelocity_{};
    float polarRadius_{0.f};
    float polarStartRadius_{0.f};
    float polarTargetRadius_{0.f};
    sf::Time polarOpenDuration_{sf::Time::Zero};
    float polarAngleRadians_{0.f};
    float radialSpeed_{0.f};
    float angularVelocityRadians_{0.f};
    sf::Vector2f tetherOrigin_{};
    float tetherOrbitRadius_{18.f};
    float tetherThrowRadius_{52.f};
    sf::Time tetherOrbitDuration_{sf::seconds(3.f)};
    sf::Time tetherExtendDuration_{sf::seconds(0.25f)};
    sf::Time tetherHoldDuration_{sf::seconds(0.08f)};
    sf::Time tetherRetractDuration_{sf::seconds(0.30f)};
    float tetherAngleRadians_{0.f};
    int damage_{1};
    std::optional<sf::Sprite> sprite_;
};
