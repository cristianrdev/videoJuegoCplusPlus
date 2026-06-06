#include "PowerUpItem.hpp"

#include <cmath>

namespace {
constexpr auto Pi = 3.14159265358979323846f;
}

PowerUpItem::PowerUpItem(sf::Vector2f position, const sf::Texture& texture, const PowerUpConfig& config)
    : startPosition_(position)
    , position_(position)
    , hitboxSize_({config.hitboxWidth, config.hitboxHeight})
    , powerUpId_(config.id)
    , lifetime_(sf::seconds(config.lifetimeSeconds))
    , flickerTime_(sf::seconds(config.flickerSeconds))
    , amplitude_(config.amplitude)
    , frequency_(config.frequency)
    , speedY_(config.speedY)
    , sprite_(texture) {
    const auto size = texture.getSize();
    sprite_.setOrigin({
        static_cast<float>(size.x / 2u),
        static_cast<float>(size.y / 2u)
    });
}

void PowerUpItem::update(sf::Time deltaTime) {
    elapsed_ += deltaTime;
    const auto seconds = elapsed_.asSeconds();
    position_.x = startPosition_.x + std::sin(seconds * frequency_ * Pi * 2.f) * amplitude_;
    position_.y += speedY_ * deltaTime.asSeconds();
}

void PowerUpItem::render(sf::RenderTarget& target) const {
    if (!shouldRender()) {
        return;
    }

    auto sprite = sprite_;
    sprite.setPosition({std::round(position_.x), std::round(position_.y)});
    target.draw(sprite);
}

bool PowerUpItem::isAlive() const {
    return elapsed_ < lifetime_;
}

sf::FloatRect PowerUpItem::hitbox() const {
    return {
        {position_.x - hitboxSize_.x * 0.5f, position_.y - hitboxSize_.y * 0.5f},
        hitboxSize_
    };
}

const std::string& PowerUpItem::powerUpId() const {
    return powerUpId_;
}

bool PowerUpItem::shouldRender() const {
    const auto remaining = lifetime_ - elapsed_;
    if (remaining > flickerTime_) {
        return true;
    }

    const auto flickerFrame = static_cast<int>(elapsed_.asSeconds() / 0.08f);
    return flickerFrame % 2 == 0;
}
