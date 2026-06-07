#include "BackgroundElement.hpp"

#include <SFML/Graphics/Color.hpp>

#include <cmath>

BackgroundElement::BackgroundElement(
    sf::Vector2f position,
    float speedY,
    const sf::Texture& texture,
    int tileIndex,
    sf::Vector2i tileSize,
    sf::Vector2f hitboxOffset,
    sf::Vector2f hitboxSize,
    int contactDamage
)
    : position_(position)
    , speedY_(speedY)
    , tileSize_(tileSize)
    , hitboxOffset_(hitboxOffset)
    , hitboxSize_(hitboxSize)
    , contactDamage_(contactDamage)
    , sprite_(texture) {
    sprite_.setTextureRect({
        {tileIndex * tileSize_.x, 0},
        tileSize_
    });
}

void BackgroundElement::update(sf::Time deltaTime) {
    position_.y += speedY_ * deltaTime.asSeconds();
}

void BackgroundElement::render(sf::RenderTarget& target, bool showHitbox) const {
    auto sprite = sprite_;
    sprite.setPosition({
        std::round(position_.x),
        std::round(position_.y)
    });
    target.draw(sprite);

    if (showHitbox && contactDamage_ > 0) {
        renderHitbox(target);
    }
}

bool BackgroundElement::isAlive(float logicalHeight) const {
    return position_.y <= logicalHeight + static_cast<float>(tileSize_.y);
}

sf::FloatRect BackgroundElement::hitbox() const {
    return {
        position_ + hitboxOffset_,
        hitboxSize_
    };
}

int BackgroundElement::contactDamage() const {
    return contactDamage_;
}

void BackgroundElement::renderHitbox(sf::RenderTarget& target) const {
    auto box = sf::RectangleShape(hitboxSize_);
    box.setPosition({
        std::round(position_.x + hitboxOffset_.x),
        std::round(position_.y + hitboxOffset_.y)
    });
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(sf::Color(60, 255, 80));
    box.setOutlineThickness(1.f);
    target.draw(box);
}
