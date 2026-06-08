#include "EnemyBullet.hpp"

#include <SFML/Graphics/Color.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <utility>

namespace {
constexpr auto Pi = 3.14159265358979323846f;

float angleDegrees(sf::Vector2f velocity) {
    return -std::atan2(velocity.x, velocity.y) * 180.f / Pi;
}

sf::Vector2f normalizedOrDown(sf::Vector2f value) {
    const auto length = std::sqrt(value.x * value.x + value.y * value.y);
    if (length <= 0.001f) {
        return {0.f, 1.f};
    }

    return {value.x / length, value.y / length};
}

float currentVisibleLength(sf::Vector2f size, sf::Time age, sf::Time growDuration, const std::string& visualType) {
    if (visualType != "pixel_line" || growDuration <= sf::Time::Zero) {
        return size.y;
    }

    const auto t = std::clamp(age.asSeconds() / growDuration.asSeconds(), 0.f, 1.f);
    return std::max(1.f, std::round(size.y * t));
}
}

EnemyBullet::EnemyBullet(sf::Vector2f position, sf::Vector2f velocity, int damage)
    : EnemyBullet(position, velocity, nullptr, damage) {
}

EnemyBullet::EnemyBullet(sf::Vector2f position, sf::Vector2f velocity, const sf::Texture* texture, int damage)
    : EnemyBullet(position, velocity, texture, damage, texture ? "sprite" : "rect", {4.f, 4.f}) {
}

EnemyBullet::EnemyBullet(
    sf::Vector2f position,
    sf::Vector2f velocity,
    const sf::Texture* texture,
    int damage,
    std::string visualType,
    sf::Vector2f visualSize,
    float visualGrowSeconds,
    int ownerInstanceId,
    bool rotateToVelocity
)
    : position_(position)
    , velocity_(velocity)
    , size_(visualSize)
    , visualType_(std::move(visualType))
    , growDuration_(sf::seconds(visualGrowSeconds))
    , ownerInstanceId_(ownerInstanceId)
    , rotateToVelocity_(rotateToVelocity)
    , damage_(damage) {
    if (texture) {
        sprite_.emplace(*texture);
        const auto textureSize = texture->getSize();
        size_ = {
            static_cast<float>(textureSize.x),
            static_cast<float>(textureSize.y)
        };
        sprite_->setOrigin({size_.x * 0.5f, size_.y * 0.5f});
    }
}

void EnemyBullet::update(sf::Time deltaTime) {
    age_ += deltaTime;
    if (visualType_ == "pixel_line") {
        return;
    }

    position_ += velocity_ * deltaTime.asSeconds();
}

void EnemyBullet::render(sf::RenderTarget& target) const {
    if (sprite_) {
        auto sprite = *sprite_;
        sprite.setPosition({std::round(position_.x), std::round(position_.y)});
        if (rotateToVelocity_) {
            sprite.setRotation(sf::degrees(angleDegrees(velocity_)));
        }
        target.draw(sprite);
        return;
    }

    if (visualType_ == "pixel_line") {
        const auto visibleLength = currentVisibleLength(size_, age_, growDuration_, visualType_);
        const auto direction = normalizedOrDown(velocity_);
        const auto center = position_ + direction * (visibleLength * 0.5f);
        auto line = sf::RectangleShape({size_.x, visibleLength});
        line.setOrigin({size_.x * 0.5f, visibleLength * 0.5f});
        line.setPosition({std::round(center.x), std::round(center.y)});
        line.setRotation(sf::degrees(angleDegrees(velocity_)));
        line.setFillColor(sf::Color(238, 244, 255));
        target.draw(line);

        auto core = sf::RectangleShape({std::max(1.f, size_.x), visibleLength});
        core.setOrigin({core.getSize().x * 0.5f, core.getSize().y * 0.5f});
        core.setPosition({std::round(center.x), std::round(center.y)});
        core.setRotation(sf::degrees(angleDegrees(velocity_)));
        core.setFillColor(sf::Color(255, 255, 255));
        target.draw(core);
        return;
    }

    auto shape = sf::RectangleShape(size_);
    shape.setOrigin({size_.x * 0.5f, size_.y * 0.5f});
    shape.setPosition({std::round(position_.x), std::round(position_.y)});
    shape.setFillColor(sf::Color(255, 72, 44));
    target.draw(shape);

    auto core = sf::RectangleShape({2.f, 2.f});
    core.setOrigin({1.f, 1.f});
    core.setPosition({std::round(position_.x), std::round(position_.y)});
    core.setFillColor(sf::Color(255, 210, 80));
    target.draw(core);
}

bool EnemyBullet::isAlive(sf::Vector2f logicalSize) const {
    if (visualType_ == "pixel_line") {
        return age_ <= growDuration_ + sf::seconds(0.28f);
    }

    return position_.x >= -8.f &&
        position_.x <= logicalSize.x + 8.f &&
        position_.y >= -8.f &&
        position_.y <= logicalSize.y + 8.f;
}

sf::FloatRect EnemyBullet::hitbox() const {
    if (visualType_ == "pixel_line") {
        const auto direction = normalizedOrDown(velocity_);
        const auto visibleLength = currentVisibleLength(size_, age_, growDuration_, visualType_);
        const auto center = position_ + direction * (visibleLength * 0.5f);
        const auto halfLength = visibleLength * 0.5f;
        const auto halfWidth = size_.x * 0.5f;
        const auto perp = sf::Vector2f{-direction.y, direction.x};
        const auto points = std::array<sf::Vector2f, 4>{
            center + direction * halfLength + perp * halfWidth,
            center + direction * halfLength - perp * halfWidth,
            center - direction * halfLength + perp * halfWidth,
            center - direction * halfLength - perp * halfWidth
        };
        auto minX = points[0].x;
        auto maxX = points[0].x;
        auto minY = points[0].y;
        auto maxY = points[0].y;
        for (const auto point : points) {
            minX = std::min(minX, point.x);
            maxX = std::max(maxX, point.x);
            minY = std::min(minY, point.y);
            maxY = std::max(maxY, point.y);
        }

        return {{minX, minY}, {maxX - minX, maxY - minY}};
    }

    return {
        {position_.x - size_.x * 0.5f, position_.y - size_.y * 0.5f},
        size_
    };
}

bool EnemyBullet::isPixelLine() const {
    return visualType_ == "pixel_line";
}

void EnemyBullet::setPosition(sf::Vector2f position) {
    position_ = position;
}

int EnemyBullet::ownerInstanceId() const {
    return ownerInstanceId_;
}

int EnemyBullet::damage() const {
    return damage_;
}
