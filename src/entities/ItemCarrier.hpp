#pragma once

#include "ItemConfigSystem.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>

class ItemCarrier {
public:
    ItemCarrier(sf::Vector2f position, const sf::Texture& texture, const ItemCarrierConfig& config);

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;
    void takeDamage(int damage);

    bool isAlive(sf::Vector2f logicalSize) const;
    bool isDestroyed() const;
    sf::FloatRect hitbox() const;
    sf::Vector2f position() const;
    const std::string& dropId() const;

private:
    sf::Vector2f position_;
    sf::Vector2f hitboxSize_;
    float speedY_{0.f};
    int health_{0};
    std::string dropId_;
    sf::Sprite sprite_;
};
