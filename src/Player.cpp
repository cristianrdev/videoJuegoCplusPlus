#include "Player.hpp"

#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <algorithm>
#include <cmath>

namespace {
constexpr auto JoystickDeadZone = 18.f;

int firstConnectedJoystick() {
    for (auto joystick = 0u; joystick < sf::Joystick::Count; ++joystick) {
        if (sf::Joystick::isConnected(joystick)) {
            return static_cast<int>(joystick);
        }
    }

    return -1;
}

float axisValue(unsigned int joystick, sf::Joystick::Axis axis) {
    if (!sf::Joystick::hasAxis(joystick, axis)) {
        return 0.f;
    }

    const auto raw = sf::Joystick::getAxisPosition(joystick, axis);
    return std::abs(raw) >= JoystickDeadZone ? raw / 100.f : 0.f;
}

sf::Vector2f joystickDirection() {
    const auto joystick = firstConnectedJoystick();
    if (joystick < 0) {
        return {0.f, 0.f};
    }

    const auto id = static_cast<unsigned int>(joystick);
    auto direction = sf::Vector2f{
        axisValue(id, sf::Joystick::Axis::X),
        axisValue(id, sf::Joystick::Axis::Y)
    };

    const auto dpadX = axisValue(id, sf::Joystick::Axis::PovX);
    const auto dpadY = axisValue(id, sf::Joystick::Axis::PovY);
    if (dpadX != 0.f) {
        direction.x = dpadX;
    }
    if (dpadY != 0.f) {
        direction.y = -dpadY;
    }

    return direction;
}
}

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

    direction += joystickDirection();

    if (direction.x < 0.f) {
        setVisualState(VisualState::Left);
    } else if (direction.x > 0.f) {
        setVisualState(VisualState::Right);
    } else {
        setVisualState(VisualState::Center);
    }

    const auto length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 1.f) {
        direction /= length;
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

sf::Vector2f Player::position() const {
    return position_;
}

sf::Vector2f Player::laserSpawnPosition() const {
    return {position_.x, position_.y - 18.f};
}

int Player::health() const {
    return health_;
}

void Player::takeDamage(int damage) {
    health_ = std::max(0, health_ - damage);
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
