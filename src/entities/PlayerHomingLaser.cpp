#include "PlayerHomingLaser.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <algorithm>
#include <cmath>

namespace {
constexpr auto Pi = 3.14159265358979323846f;

float length(sf::Vector2f value) {
    return std::sqrt(value.x * value.x + value.y * value.y);
}

sf::Vector2f normalizedOr(sf::Vector2f value, sf::Vector2f fallback) {
    const auto valueLength = length(value);
    if (valueLength <= 0.001f) {
        const auto fallbackLength = length(fallback);
        if (fallbackLength <= 0.001f) {
            return {0.f, -1.f};
        }
        return {fallback.x / fallbackLength, fallback.y / fallbackLength};
    }

    return {value.x / valueLength, value.y / valueLength};
}

float angleDegrees(sf::Vector2f velocity) {
    return -std::atan2(velocity.x, velocity.y) * 180.f / Pi;
}
}

PlayerHomingLaser::PlayerHomingLaser(
    sf::Vector2f position,
    int targetInstanceId,
    float sideSign,
    float speed,
    float turnRate,
    float sideExitSeconds,
    float sideExitSpeed,
    float damage,
    sf::Vector2f hitboxSize,
    sf::Vector2f visualSize,
    float coreWidth,
    float glowRadius
)
    : position_(position)
    , velocity_({sideSign * sideExitSpeed, 0.f})
    , hitboxSize_(hitboxSize)
    , visualSize_(visualSize)
    , targetInstanceId_(targetInstanceId)
    , sideSign_(sideSign)
    , speed_(speed)
    , turnRate_(turnRate)
    , sideExitSeconds_(sideExitSeconds)
    , sideExitSpeed_(sideExitSpeed)
    , damage_(damage)
    , coreWidth_(coreWidth)
    , glowRadius_(glowRadius) {
}

void PlayerHomingLaser::update(sf::Time deltaTime, sf::Vector2f targetPosition, bool targetAlive) {
    if (!alive_) {
        return;
    }

    age_ += deltaTime;
    const auto seconds = deltaTime.asSeconds();
    if (age_.asSeconds() < sideExitSeconds_) {
        velocity_ = {sideSign_ * sideExitSpeed_, 0.f};
        position_ += velocity_ * seconds;
        return;
    }

    if (!targetAlive) {
        position_ += velocity_ * seconds;
        return;
    }

    const auto desired = normalizedOr(targetPosition - position_, velocity_) * speed_;
    const auto blend = std::clamp(turnRate_ * seconds, 0.f, 1.f);
    velocity_ = {
        velocity_.x + (desired.x - velocity_.x) * blend,
        velocity_.y + (desired.y - velocity_.y) * blend
    };
    velocity_ = normalizedOr(velocity_, desired) * speed_;
    position_ += velocity_ * seconds;
}

void PlayerHomingLaser::render(sf::RenderTarget& target) const {
    if (!alive_) {
        return;
    }

    const auto angle = sf::degrees(angleDegrees(velocity_));
    auto glow = sf::CircleShape(glowRadius_, 16);
    glow.setOrigin({glowRadius_, glowRadius_});
    glow.setPosition({std::round(position_.x), std::round(position_.y)});
    glow.setFillColor(sf::Color(48, 168, 255, 86));
    target.draw(glow);

    auto body = sf::RectangleShape(visualSize_);
    body.setOrigin({visualSize_.x * 0.5f, visualSize_.y * 0.5f});
    body.setPosition({std::round(position_.x), std::round(position_.y)});
    body.setRotation(angle + sf::degrees(90.f));
    body.setFillColor(sf::Color(196, 240, 255));
    target.draw(body);

    const auto coreSize = sf::Vector2f{
        std::max(1.f, visualSize_.x - 4.f),
        std::max(1.f, coreWidth_)
    };
    auto core = sf::RectangleShape(coreSize);
    core.setOrigin({coreSize.x * 0.5f, coreSize.y * 0.5f});
    core.setPosition({std::round(position_.x), std::round(position_.y)});
    core.setRotation(angle + sf::degrees(90.f));
    core.setFillColor(sf::Color(255, 255, 255));
    target.draw(core);
}

bool PlayerHomingLaser::isAlive(sf::Vector2f logicalSize) const {
    return alive_ &&
        position_.x >= -24.f &&
        position_.x <= logicalSize.x + 24.f &&
        position_.y >= -24.f &&
        position_.y <= logicalSize.y + 24.f;
}

void PlayerHomingLaser::destroy() {
    alive_ = false;
}

sf::FloatRect PlayerHomingLaser::hitbox() const {
    return {
        {position_.x - hitboxSize_.x * 0.5f, position_.y - hitboxSize_.y * 0.5f},
        hitboxSize_
    };
}

int PlayerHomingLaser::targetInstanceId() const {
    return targetInstanceId_;
}

float PlayerHomingLaser::damage() const {
    return damage_;
}
