#include "Enemy.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <algorithm>
#include <cmath>
#include <utility>

namespace {
int nextEnemyInstanceId() {
    static auto nextId = 1;
    return nextId++;
}

sf::Shader* negativeShader() {
    static auto shader = sf::Shader{};
    static auto initialized = false;
    static auto available = false;

    if (!initialized) {
        initialized = true;
        if (sf::Shader::isAvailable()) {
            available = shader.loadFromMemory(
                "uniform sampler2D texture;\n"
                "void main()\n"
                "{\n"
                "    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);\n"
                "    gl_FragColor = vec4(1.0 - pixel.rgb, pixel.a) * gl_Color;\n"
                "}\n",
                sf::Shader::Type::Fragment
            );

            if (available) {
                shader.setUniform("texture", sf::Shader::CurrentTexture);
            }
        }
    }

    return available ? &shader : nullptr;
}
}

Enemy::Enemy(
    sf::Vector2f position,
    const sf::Texture& texture,
    std::string enemyId,
    std::string patternId,
    std::string movementId,
    int health,
    int contactDamage,
    std::string hitboxShape,
    sf::Vector2f configuredHitboxSize,
    sf::Vector2f configuredHitboxOffset,
    bool blinkEnabled,
    int blinkHealthThreshold
)
    : startPosition_(position)
    , position_(position)
    , health_(health)
    , contactDamage_(contactDamage)
    , hitboxShape_(std::move(hitboxShape))
    , instanceId_(nextEnemyInstanceId())
    , blinkEnabled_(blinkEnabled)
    , blinkHealthThreshold_(blinkHealthThreshold)
    , enemyId_(std::move(enemyId))
    , patternId_(std::move(patternId))
    , movementId_(std::move(movementId))
    , sprite_(texture) {
    if (enemyId_ == "enemy_robot_fish") {
        size_ = {82.f, 252.f};
        visualSize_ = {120.f, 300.f};
        sprite_.setTextureRect({
            {0, 0},
            {static_cast<int>(visualSize_.x), static_cast<int>(visualSize_.y)}
        });
    } else if (enemyId_ == "enemy_mech_spider_mother") {
        size_ = {42.f, 48.f};
        visualSize_ = {50.f, 60.f};
        sprite_.setTextureRect({
            {0, 0},
            {static_cast<int>(visualSize_.x), static_cast<int>(visualSize_.y)}
        });
    } else if (enemyId_ == "enemy_mech_spider") {
        size_ = {32.f, 32.f};
        visualSize_ = {32.f, 32.f};
        sprite_.setTextureRect({
            {0, 0},
            {static_cast<int>(visualSize_.x), static_cast<int>(visualSize_.y)}
        });
    } else {
        const auto textureSize = texture.getSize();
        visualSize_ = {
            static_cast<float>(textureSize.x),
            static_cast<float>(textureSize.y)
        };
    }

    if (configuredHitboxSize.x > 0.f && configuredHitboxSize.y > 0.f) {
        size_ = configuredHitboxSize;
    }
    hitboxOffset_ = configuredHitboxOffset;

    sprite_.setOrigin({
        visualSize_.x * 0.5f,
        visualSize_.y * 0.5f
    });
}

void Enemy::update(sf::Time deltaTime) {
    elapsed_ += deltaTime;
    fireTimer_ -= deltaTime;
    firingVisualTime_ -= deltaTime;
    if (firingVisualTime_ < sf::Time::Zero) {
        firingVisualTime_ = sf::Time::Zero;
    }
}

void Enemy::render(sf::RenderTarget& target, bool showHitbox) const {
    auto pixelSnappedSprite = sprite_;
    if (enemyId_ == "enemy_robot_fish") {
        const auto frameIndex = firingVisualTime_ > sf::Time::Zero ? 1 : 0;
        pixelSnappedSprite.setTextureRect({
            {frameIndex * static_cast<int>(visualSize_.x), 0},
            {static_cast<int>(visualSize_.x), static_cast<int>(visualSize_.y)}
        });
    } else if (enemyId_ == "enemy_mech_spider") {
        const auto frameIndex = static_cast<int>(elapsed_.asSeconds() / 0.12f) % 2;
        pixelSnappedSprite.setTextureRect({
            {frameIndex * static_cast<int>(visualSize_.x), 0},
            {static_cast<int>(visualSize_.x), static_cast<int>(visualSize_.y)}
        });
    } else if (enemyId_ == "enemy_mech_spider_mother") {
        const auto frameIndex = static_cast<int>(elapsed_.asSeconds() / 0.14f) % 3;
        pixelSnappedSprite.setTextureRect({
            {frameIndex * static_cast<int>(visualSize_.x), 0},
            {static_cast<int>(visualSize_.x), static_cast<int>(visualSize_.y)}
        });
    }
    pixelSnappedSprite.setPosition({std::round(position_.x), std::round(position_.y)});

    if (shouldRenderNegative()) {
        if (auto* shader = negativeShader()) {
            auto states = sf::RenderStates{};
            states.shader = shader;
            target.draw(pixelSnappedSprite, states);
            if (showHitbox) {
                renderHitbox(target);
            }
            return;
        }
    }

    target.draw(pixelSnappedSprite);
    if (showHitbox) {
        renderHitbox(target);
    }
}

void Enemy::takeDamage(int damage) {
    health_ = std::max(0, health_ - damage);
}

void Enemy::setPosition(sf::Vector2f position) {
    position_ = position;
}

bool Enemy::isAlive() const {
    return health_ > 0 && position_.y - visualSize_.y * 0.5f < 344.f;
}

bool Enemy::shouldFire() const {
    return !patternId_.empty() && patternId_ != "none" && fireTimer_ <= sf::Time::Zero;
}

int Enemy::contactDamage() const {
    return contactDamage_;
}

void Enemy::resetFireTimer(float intervalSeconds) {
    fireTimer_ = sf::seconds(intervalSeconds);
}

void Enemy::startFiringVisual(sf::Time duration) {
    firingVisualTime_ = duration;
}

bool Enemy::shouldRenderNegative() const {
    if (!blinkEnabled_ || blinkHealthThreshold_ <= 0 || health_ >= blinkHealthThreshold_) {
        return false;
    }

    const auto blinkFrame = static_cast<int>(elapsed_.asSeconds() / 0.06f);
    return blinkFrame % 2 == 0;
}

sf::Vector2f Enemy::bulletSpawnPosition() const {
    if (enemyId_ == "enemy_robot_fish") {
        return {position_.x, position_.y + 132.f};
    }

    if (enemyId_ == "enemy_mech_spider_mother") {
        return {position_.x, position_.y + 24.f};
    }

    return {position_.x, position_.y + size_.y * 0.5f};
}

sf::Vector2f Enemy::position() const {
    return position_;
}

sf::Vector2f Enemy::startPosition() const {
    return startPosition_;
}

sf::Time Enemy::elapsed() const {
    return elapsed_;
}

sf::FloatRect Enemy::hitbox() const {
    return {
        {
            position_.x + hitboxOffset_.x - size_.x * 0.5f,
            position_.y + hitboxOffset_.y - size_.y * 0.5f
        },
        size_
    };
}

bool Enemy::intersects(sf::FloatRect rect) const {
    const auto bounds = hitbox();
    if (hitboxShape_ != "oval") {
        return bounds.position.x < rect.position.x + rect.size.x &&
            bounds.position.x + bounds.size.x > rect.position.x &&
            bounds.position.y < rect.position.y + rect.size.y &&
            bounds.position.y + bounds.size.y > rect.position.y;
    }

    const auto radiusX = bounds.size.x * 0.5f;
    const auto radiusY = bounds.size.y * 0.5f;
    if (radiusX <= 0.f || radiusY <= 0.f) {
        return false;
    }

    const auto center = sf::Vector2f{
        bounds.position.x + radiusX,
        bounds.position.y + radiusY
    };
    const auto nearest = sf::Vector2f{
        std::clamp(center.x, rect.position.x, rect.position.x + rect.size.x),
        std::clamp(center.y, rect.position.y, rect.position.y + rect.size.y)
    };
    const auto normalizedX = (nearest.x - center.x) / radiusX;
    const auto normalizedY = (nearest.y - center.y) / radiusY;
    return normalizedX * normalizedX + normalizedY * normalizedY <= 1.f;
}

int Enemy::instanceId() const {
    return instanceId_;
}

void Enemy::renderHitbox(sf::RenderTarget& target) const {
    const auto bounds = hitbox();
    if (hitboxShape_ == "oval") {
        auto oval = sf::CircleShape(bounds.size.x * 0.5f, 48);
        oval.setScale({1.f, bounds.size.y / std::max(1.f, bounds.size.x)});
        oval.setPosition({
            std::round(bounds.position.x),
            std::round(bounds.position.y)
        });
        oval.setFillColor(sf::Color::Transparent);
        oval.setOutlineColor(sf::Color(60, 255, 80));
        oval.setOutlineThickness(1.f);
        target.draw(oval);
        return;
    }

    auto box = sf::RectangleShape(bounds.size);
    box.setPosition({
        std::round(bounds.position.x),
        std::round(bounds.position.y)
    });
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(sf::Color(60, 255, 80));
    box.setOutlineThickness(1.f);
    target.draw(box);
}

const std::string& Enemy::enemyId() const {
    return enemyId_;
}

const std::string& Enemy::patternId() const {
    return patternId_;
}

const std::string& Enemy::movementId() const {
    return movementId_;
}
