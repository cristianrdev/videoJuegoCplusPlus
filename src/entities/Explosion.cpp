#include "Explosion.hpp"

#include <algorithm>
#include <cmath>

Explosion::Explosion(sf::Vector2f position, const sf::Texture& texture)
    : Explosion(position, texture, {32, 32}, 3, sf::seconds(0.07f)) {
}

Explosion::Explosion(
    sf::Vector2f position,
    const sf::Texture& texture,
    sf::Vector2i frameSize,
    int frameCount,
    sf::Time frameDuration
)
    : position_(position)
    , frameDuration_(frameDuration)
    , frameCount_(frameCount)
    , frameSize_(frameSize)
    , sprite_(texture) {
    sprite_.setTextureRect({{0, 0}, frameSize_});
    sprite_.setOrigin({
        static_cast<float>(frameSize_.x) * 0.5f,
        static_cast<float>(frameSize_.y) * 0.5f
    });
}

void Explosion::update(sf::Time deltaTime) {
    elapsed_ += deltaTime;
    currentFrame_ = std::clamp(
        static_cast<int>(elapsed_.asSeconds() / frameDuration_.asSeconds()),
        0,
        frameCount_ - 1
    );
}

void Explosion::render(sf::RenderTarget& target) const {
    auto pixelSnappedSprite = sprite_;
    pixelSnappedSprite.setTextureRect({{currentFrame_ * frameSize_.x, 0}, frameSize_});
    pixelSnappedSprite.setPosition({std::round(position_.x), std::round(position_.y)});
    target.draw(pixelSnappedSprite);
}

bool Explosion::isFinished() const {
    return elapsed_ >= frameDuration_ * static_cast<float>(frameCount_);
}
