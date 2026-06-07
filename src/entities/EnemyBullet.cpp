#include "EnemyBullet.hpp"

#include <SFML/Graphics/Color.hpp>

#include <cmath>

EnemyBullet::EnemyBullet(sf::Vector2f position, sf::Vector2f velocity, int damage)
    : EnemyBullet(position, velocity, nullptr, damage) {
}

EnemyBullet::EnemyBullet(sf::Vector2f position, sf::Vector2f velocity, const sf::Texture* texture, int damage)
    : position_(position)
    , velocity_(velocity)
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
    position_ += velocity_ * deltaTime.asSeconds();
}

void EnemyBullet::render(sf::RenderTarget& target) const {
    if (sprite_) {
        auto sprite = *sprite_;
        sprite.setPosition({std::round(position_.x), std::round(position_.y)});
        target.draw(sprite);
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
    return position_.x >= -8.f &&
        position_.x <= logicalSize.x + 8.f &&
        position_.y >= -8.f &&
        position_.y <= logicalSize.y + 8.f;
}

sf::FloatRect EnemyBullet::hitbox() const {
    return {
        {position_.x - size_.x * 0.5f, position_.y - size_.y * 0.5f},
        size_
    };
}

int EnemyBullet::damage() const {
    return damage_;
}
