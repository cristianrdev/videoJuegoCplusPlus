#include "BackgroundElement.hpp"

#include <SFML/Graphics/Color.hpp>

#include <algorithm>
#include <cmath>
#include <utility>

BackgroundElement::BackgroundElement(
    sf::Vector2f position,
    float speedY,
    const sf::Texture& texture,
    int tileIndex,
    sf::Vector2i tileSize,
    std::string hitboxShape,
    sf::Vector2f hitboxOffset,
    sf::Vector2f hitboxSize,
    int contactDamage
)
    : position_(position)
    , speedY_(speedY)
    , tileSize_(tileSize)
    , hitboxShape_(std::move(hitboxShape))
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

bool BackgroundElement::intersects(sf::FloatRect rect) const {
    const auto bounds = hitbox();
    if (hitboxShape_ != "oval") {
        return bounds.position.x < rect.position.x + rect.size.x &&
            bounds.position.x + bounds.size.x > rect.position.x &&
            bounds.position.y < rect.position.y + rect.size.y &&
            bounds.position.y + bounds.size.y > rect.position.y;
    }

    const auto radiusX = bounds.size.x * 0.5f;
    const auto radiusY = bounds.size.y * 0.5f;
    if (radiusX <= 0.f || radiusY <= 0.f) {
        return false;
    }

    const auto center = sf::Vector2f{
        bounds.position.x + radiusX,
        bounds.position.y + radiusY
    };
    const auto nearest = sf::Vector2f{
        std::clamp(center.x, rect.position.x, rect.position.x + rect.size.x),
        std::clamp(center.y, rect.position.y, rect.position.y + rect.size.y)
    };
    const auto normalizedX = (nearest.x - center.x) / radiusX;
    const auto normalizedY = (nearest.y - center.y) / radiusY;
    return normalizedX * normalizedX + normalizedY * normalizedY <= 1.f;
}

int BackgroundElement::contactDamage() const {
    return contactDamage_;
}

void BackgroundElement::renderHitbox(sf::RenderTarget& target) const {
    if (hitboxShape_ == "oval") {
        auto oval = sf::CircleShape(hitboxSize_.x * 0.5f, 48);
        oval.setScale({1.f, hitboxSize_.y / std::max(1.f, hitboxSize_.x)});
        oval.setPosition({
            std::round(position_.x + hitboxOffset_.x),
            std::round(position_.y + hitboxOffset_.y)
        });
        oval.setFillColor(sf::Color::Transparent);
        oval.setOutlineColor(sf::Color(60, 255, 80));
        oval.setOutlineThickness(1.f);
        target.draw(oval);
        return;
    }

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
