#include "Player.hpp"

#include <SFML/Window/Keyboard.hpp>

#include <algorithm>

Player::Player(AssetManager& assets, sf::Vector2f logicalSize)
    : logicalSize_(logicalSize)
    , position_({logicalSize.x * 0.5f, logicalSize.y - 40.f})
    , centerTexture_(&assets.getTexture("player_ship_center"))
    , leftTexture_(&assets.getTexture("player_ship_left"))
    , rightTexture_(&assets.getTexture("player_ship_right"))
    , sprite_(*centerTexture_) {
    sprite_.setOrigin({16.f, 16.f});
    sprite_.setPosition(position_);
}

void Player::update(sf::Time deltaTime) {
    auto direction = sf::Vector2f{0.f, 0.f};

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        direction.x -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        direction.x += 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        direction.y -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        direction.y += 1.f;
    }

    if (direction.x < 0.f) {
        setVisualState(VisualState::Left);
    } else if (direction.x > 0.f) {
        setVisualState(VisualState::Right);
    } else {
        setVisualState(VisualState::Center);
    }

    if (direction.x != 0.f && direction.y != 0.f) {
        direction *= 0.70710678f;
    }

    position_ += direction * speed_ * deltaTime.asSeconds();
    clampToLogicalArea();
    sprite_.setPosition(position_);
}

void Player::render(sf::RenderTarget& target) const {
    target.draw(sprite_);
}

sf::FloatRect Player::hitbox() const {
    return {
        {position_.x - hitboxSize_.x * 0.5f, position_.y - hitboxSize_.y * 0.5f},
        hitboxSize_
    };
}

void Player::setVisualState(VisualState state) {
    if (visualState_ == state) {
        return;
    }

    visualState_ = state;
    switch (visualState_) {
    case VisualState::Center:
        sprite_.setTexture(*centerTexture_, true);
        break;
    case VisualState::Left:
        sprite_.setTexture(*leftTexture_, true);
        break;
    case VisualState::Right:
        sprite_.setTexture(*rightTexture_, true);
        break;
    }

    sprite_.setOrigin({16.f, 16.f});
}

void Player::clampToLogicalArea() {
    constexpr auto halfSprite = 16.f;
    position_.x = std::clamp(position_.x, halfSprite, logicalSize_.x - halfSprite);
    position_.y = std::clamp(position_.y, halfSprite, logicalSize_.y - halfSprite);
}
