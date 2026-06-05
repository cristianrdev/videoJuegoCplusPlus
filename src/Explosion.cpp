#include "Explosion.hpp"

#include <algorithm>
#include <cmath>

Explosion::Explosion(sf::Vector2f position, const sf::Texture& texture)
    : position_(position)
    , sprite_(texture) {
    sprite_.setTextureRect({{0, 0}, {32, 32}});
    sprite_.setOrigin({16.f, 16.f});
}

void Explosion::update(sf::Time deltaTime) {
    elapsed_ += deltaTime;
    currentFrame_ = std::clamp(
        static_cast<int>(elapsed_.asSeconds() / frameDuration_.asSeconds()),
        0,
        2
    );
}

void Explosion::render(sf::RenderTarget& target) const {
    auto pixelSnappedSprite = sprite_;
    pixelSnappedSprite.setTextureRect({{currentFrame_ * 32, 0}, {32, 32}});
    pixelSnappedSprite.setPosition({std::round(position_.x), std::round(position_.y)});
    target.draw(pixelSnappedSprite);
}

bool Explosion::isFinished() const {
    return elapsed_ >= frameDuration_ * 3.f;
}
