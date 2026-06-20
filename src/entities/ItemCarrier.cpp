#include "ItemCarrier.hpp"

#include <algorithm>
#include <cmath>

ItemCarrier::ItemCarrier(sf::Vector2f position, const sf::Texture& texture, const ItemCarrierConfig& config)
    : position_(position)
    , hitboxSize_({config.hitboxWidth, config.hitboxHeight})
    , speedY_(config.speedY)
    , health_(config.health)
    , dropId_(config.dropId)
    , sprite_(texture) {
    const auto size = texture.getSize();
    sprite_.setOrigin({
        static_cast<float>(size.x / 2u),
        static_cast<float>(size.y / 2u)
    });
}

void ItemCarrier::update(sf::Time deltaTime) {
    position_.y += speedY_ * deltaTime.asSeconds();
}

void ItemCarrier::render(sf::RenderTarget& target) const {
    auto sprite = sprite_;
    sprite.setPosition({std::round(position_.x), std::round(position_.y)});
    target.draw(sprite);
}

void ItemCarrier::takeDamage(float damage) {
    health_ = std::max(0.f, health_ - damage);
}

bool ItemCarrier::isAlive(sf::Vector2f logicalSize) const {
    return health_ > 0 && position_.y - hitboxSize_.y * 0.5f <= logicalSize.y + hitboxSize_.y;
}

bool ItemCarrier::isDestroyed() const {
    return health_ <= 0;
}

sf::FloatRect ItemCarrier::hitbox() const {
    return {
        {position_.x - hitboxSize_.x * 0.5f, position_.y - hitboxSize_.y * 0.5f},
        hitboxSize_
    };
}

sf::Vector2f ItemCarrier::position() const {
    return position_;
}

const std::string& ItemCarrier::dropId() const {
    return dropId_;
}
