#pragma once

#include "AssetManager.hpp"
#include "PlayerConfigSystem.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

class Player {
public:
    Player(AssetManager& assets, sf::Vector2f logicalSize, const PlayerConfig& config);

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;

    sf::FloatRect hitbox() const;
    sf::Vector2f position() const;
    sf::Vector2f laserSpawnPosition() const;
    int health() const;
    bool isAlive() const;
    bool isInvincible() const;
    bool takeDamage(int damage);
    void collectPowerUpP();
    int projectileCount() const;

private:
    enum class VisualState {
        Center,
        Left,
        Right
    };

    void setVisualState(VisualState state);
    void clampToLogicalArea();
    void updateSpriteFrame();
    void renderThrusters(sf::RenderTarget& target) const;

    sf::Vector2f logicalSize_;
    sf::Vector2f position_;
    PlayerConfig config_;
    sf::Vector2f hitboxSize_{6.f, 6.f};
    sf::Vector2i frameSize_{32, 32};
    sf::Vector2i thrusterFrameSize_{8, 12};
    int health_{0};
    int projectileCount_{1};
    float verticalThrust_{0.f};
    sf::Time thrusterAnimationElapsed_{sf::Time::Zero};
    sf::Time invincibilityRemaining_{sf::Time::Zero};
    sf::Time invincibilityElapsed_{sf::Time::Zero};

    const sf::Texture* spriteSheetTexture_{nullptr};
    const sf::Texture* thrusterTexture_{nullptr};
    VisualState visualState_{VisualState::Center};
    sf::Sprite sprite_;
};
