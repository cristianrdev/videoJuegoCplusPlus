#pragma once

#include "ItemConfigSystem.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <optional>
#include <string>

class PowerUpItem {
public:
    PowerUpItem(sf::Vector2f position, const sf::Texture& texture, const PowerUpConfig& config);
    PowerUpItem(
        sf::Vector2f position,
        const sf::Texture& texture,
        const PowerUpConfig& config,
        const sf::Texture& alternateTexture,
        const std::string& alternatePowerUpId
    );

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;

    bool isAlive() const;
    sf::FloatRect hitbox() const;
    const std::string& powerUpId() const;

private:
    bool shouldRender() const;

    sf::Vector2f startPosition_;
    sf::Vector2f position_;
    sf::Vector2f hitboxSize_;
    std::string powerUpId_;
    std::string basePowerUpId_;
    std::string alternatePowerUpId_{"none"};
    sf::Time elapsed_{sf::Time::Zero};
    sf::Time lifetime_{sf::seconds(30.f)};
    sf::Time flickerTime_{sf::seconds(10.f)};
    sf::Time transformInterval_{sf::Time::Zero};
    float amplitude_{0.f};
    float frequency_{0.f};
    float speedY_{0.f};
    sf::Sprite sprite_;
    std::optional<sf::Sprite> alternateSprite_;
};
