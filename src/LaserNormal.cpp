#include "LaserNormal.hpp"

#include <cmath>

LaserNormal::LaserNormal(sf::Vector2f position, const sf::Texture& texture)
    : position_(position)
    , sprite_(texture) {
    const auto textureSize = texture.getSize();
    size_ = {
        static_cast<float>(textureSize.x),
        static_cast<float>(textureSize.y)
    };
    sprite_.setOrigin({
        size_.x * 0.5f,
        size_.y * 0.5f
    });
}

void LaserNormal::update(sf::Time deltaTime) {
    position_.y -= speed_ * deltaTime.asSeconds();
}

void LaserNormal::render(sf::RenderTarget& target) const {
    auto pixelSnappedSprite = sprite_;
    pixelSnappedSprite.setPosition({std::round(position_.x), std::round(position_.y)});
    target.draw(pixelSnappedSprite);
}

bool LaserNormal::isAlive() const {
    return position_.y + size_.y * 0.5f >= 0.f;
}

sf::FloatRect LaserNormal::hitbox() const {
    return {
        {position_.x - size_.x * 0.5f, position_.y - size_.y * 0.5f},
        size_
    };
}
