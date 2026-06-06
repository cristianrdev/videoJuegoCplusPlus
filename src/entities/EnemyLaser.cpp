#include "EnemyLaser.hpp"

#include <algorithm>
#include <cmath>

EnemyLaser::EnemyLaser(sf::Vector2f origin, float length, sf::Time duration, const sf::Texture& texture)
    : origin_(origin)
    , size_({static_cast<float>(texture.getSize().x), length})
    , remaining_(duration)
    , sprite_(texture) {
    const auto textureHeight = static_cast<int>(texture.getSize().y);
    const auto beamHeight = std::max(0, std::min(static_cast<int>(std::round(length)), textureHeight));
    sprite_.setTextureRect({
        {0, 0},
        {static_cast<int>(texture.getSize().x), beamHeight}
    });
    sprite_.setOrigin({size_.x * 0.5f, 0.f});
}

void EnemyLaser::update(sf::Time deltaTime) {
    remaining_ -= deltaTime;
}

void EnemyLaser::render(sf::RenderTarget& target) const {
    auto sprite = sprite_;
    sprite.setPosition({
        std::round(origin_.x),
        std::round(origin_.y)
    });
    target.draw(sprite);
}

bool EnemyLaser::isAlive() const {
    return remaining_ > sf::Time::Zero;
}

bool EnemyLaser::canHitPlayer() const {
    return !hasHitPlayer_;
}

void EnemyLaser::markPlayerHit() {
    hasHitPlayer_ = true;
}

sf::FloatRect EnemyLaser::hitbox() const {
    return {
        {origin_.x - 5.f, origin_.y},
        {10.f, size_.y}
    };
}
