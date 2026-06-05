#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

class Explosion {
public:
    Explosion(sf::Vector2f position, const sf::Texture& texture);

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target) const;

    bool isFinished() const;

private:
    sf::Vector2f position_;
    sf::Time elapsed_{sf::Time::Zero};
    sf::Time frameDuration_{sf::seconds(0.07f)};
    int currentFrame_{0};
    sf::Sprite sprite_;
};
