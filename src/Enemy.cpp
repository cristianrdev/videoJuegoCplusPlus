#include "Enemy.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

Enemy::Enemy(sf::Vector2f position, const sf::Texture& texture, std::string patternId)
    : position_(position)
    , patternId_(std::move(patternId))
    , sprite_(texture) {
    const auto textureSize = texture.getSize();
    sprite_.setOrigin({
        static_cast<float>(textureSize.x) * 0.5f,
        static_cast<float>(textureSize.y) * 0.5f
    });
}

void Enemy::update(sf::Time deltaTime) {
    position_.y += driftSpeed_ * deltaTime.asSeconds();
    fireTimer_ -= deltaTime;
}

void Enemy::render(sf::RenderTarget& target) const {
    auto pixelSnappedSprite = sprite_;
    pixelSnappedSprite.setPosition({std::round(position_.x), std::round(position_.y)});
    target.draw(pixelSnappedSprite);
}

void Enemy::takeDamage(int damage) {
    health_ = std::max(0, health_ - damage);
}

bool Enemy::isAlive() const {
    return health_ > 0 && position_.y < 344.f;
}

bool Enemy::shouldFire() const {
    return fireTimer_ <= sf::Time::Zero;
}

void Enemy::resetFireTimer(float intervalSeconds) {
    fireTimer_ = sf::seconds(intervalSeconds);
}

sf::Vector2f Enemy::bulletSpawnPosition() const {
    return {position_.x, position_.y + size_.y * 0.5f};
}

sf::Vector2f Enemy::position() const {
    return position_;
}

sf::FloatRect Enemy::hitbox() const {
    return {
        {position_.x - size_.x * 0.5f, position_.y - size_.y * 0.5f},
        size_
    };
}

const std::string& Enemy::patternId() const {
    return patternId_;
}
