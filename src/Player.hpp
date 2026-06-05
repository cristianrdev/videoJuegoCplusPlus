#pragma once

#include "AssetManager.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

class Player {
public:
    Player(AssetManager& assets, sf::Vector2f logicalSize);

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;

    sf::FloatRect hitbox() const;
    sf::Vector2f position() const;
    sf::Vector2f laserSpawnPosition() const;
    int health() const;
    void takeDamage(int damage);

private:
    enum class VisualState {
        Center,
        Left,
        Right
    };

    void setVisualState(VisualState state);
    void clampToLogicalArea();
    void updateSpriteFrame();

    sf::Vector2f logicalSize_;
    sf::Vector2f position_;
    sf::Vector2f hitboxSize_{6.f, 6.f};
    sf::Vector2i frameSize_{32, 32};
    int health_{3};
    float speed_{120.f};

    const sf::Texture* spriteSheetTexture_{nullptr};
    VisualState visualState_{VisualState::Center};
    sf::Sprite sprite_;
};
