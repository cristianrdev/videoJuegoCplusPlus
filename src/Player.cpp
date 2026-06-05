#include "Player.hpp"

#include <SFML/Window/Keyboard.hpp>

#include <algorithm>
#include <cmath>

Player::Player(AssetManager& assets, sf::Vector2f logicalSize)
    : logicalSize_(logicalSize)
    , position_({logicalSize.x * 0.5f, logicalSize.y - 40.f})
    , spriteSheetTexture_(&assets.getTexture("player_ship_sheet"))
    , sprite_(*spriteSheetTexture_) {
    const auto textureSize = spriteSheetTexture_->getSize();
    frameSize_ = {
        static_cast<int>(textureSize.x / 3u),
        static_cast<int>(textureSize.y)
    };
    updateSpriteFrame();
    sprite_.setOrigin({
        static_cast<float>(frameSize_.x) * 0.5f,
        static_cast<float>(frameSize_.y) * 0.5f
    });
    sprite_.setScale({
        32.f / static_cast<float>(frameSize_.x),
        32.f / static_cast<float>(frameSize_.y)
    });
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
    auto pixelSnappedSprite = sprite_;
    pixelSnappedSprite.setPosition({
        std::round(position_.x),
        std::round(position_.y)
    });
    target.draw(pixelSnappedSprite);
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
    updateSpriteFrame();
}

void Player::clampToLogicalArea() {
    constexpr auto halfSprite = 16.f;
    position_.x = std::clamp(position_.x, halfSprite, logicalSize_.x - halfSprite);
    position_.y = std::clamp(position_.y, halfSprite, logicalSize_.y - halfSprite);
}

void Player::updateSpriteFrame() {
    auto frameIndex = 0;
    switch (visualState_) {
    case VisualState::Center:
        frameIndex = 1;
        break;
    case VisualState::Left:
        frameIndex = 0;
        break;
    case VisualState::Right:
        frameIndex = 2;
        break;
    }

    sprite_.setTextureRect({
        {frameIndex * frameSize_.x, 0},
        frameSize_
    });
}
