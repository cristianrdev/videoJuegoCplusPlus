#include "BackgroundElement.hpp"

#include <cmath>

BackgroundElement::BackgroundElement(
    sf::Vector2f position,
    float speedY,
    const sf::Texture& texture,
    int tileIndex,
    sf::Vector2i tileSize
)
    : position_(position)
    , speedY_(speedY)
    , tileSize_(tileSize)
    , sprite_(texture) {
    sprite_.setTextureRect({
        {tileIndex * tileSize_.x, 0},
        tileSize_
    });
}

void BackgroundElement::update(sf::Time deltaTime) {
    position_.y += speedY_ * deltaTime.asSeconds();
}

void BackgroundElement::render(sf::RenderTarget& target) const {
    auto sprite = sprite_;
    sprite.setPosition({
        std::round(position_.x),
        std::round(position_.y)
    });
    target.draw(sprite);
}

bool BackgroundElement::isAlive(float logicalHeight) const {
    return position_.y <= logicalHeight + static_cast<float>(tileSize_.y);
}
