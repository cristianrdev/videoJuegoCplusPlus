#include "WallBoss.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <algorithm>
#include <cmath>

namespace {
int nextWallBossCrystalInstanceId() {
    static auto nextId = 100000;
    return nextId++;
}

bool intersects(sf::FloatRect left, sf::FloatRect right) {
    return left.position.x < right.position.x + right.size.x &&
        left.position.x + left.size.x > right.position.x &&
        left.position.y < right.position.y + right.size.y &&
        left.position.y + left.size.y > right.position.y;
}
}

WallBoss::WallBoss(
    sf::Vector2f position,
    const sf::Texture& texture,
    const WallBossConfigSystem::WallBossConfig& config
)
    : position_(position)
    , size_(config.size)
    , speedY_(config.speedY)
    , holeWidth_(config.holeWidth)
    , contactDamage_(config.contactDamage)
    , texture_(&texture) {
    crystals_.reserve(config.crystals.size());
    for (const auto& crystalConfig : config.crystals) {
        auto crystal = Crystal{};
        crystal.instanceId = nextWallBossCrystalInstanceId();
        crystal.id = crystalConfig.id;
        crystal.patternId = crystalConfig.patternId;
        crystal.offset = crystalConfig.offset;
        crystal.hitboxSize = crystalConfig.hitboxSize;
        crystal.health = crystalConfig.health;
        crystal.alive = crystal.health > 0.f;
        crystals_.push_back(std::move(crystal));
    }
}

void WallBoss::update(sf::Time deltaTime) {
    position_.y += speedY_ * deltaTime.asSeconds();
    for (auto& crystal : crystals_) {
        crystal.fireTimer -= deltaTime;
    }
}

void WallBoss::render(sf::RenderTarget& target, bool showHitbox) const {
    const auto topLeft = sf::Vector2f{
        std::round(position_.x - size_.x * 0.5f),
        std::round(position_.y - size_.y * 0.5f)
    };

    auto segmentStart = 0.f;
    auto drawSegment = [&](float from, float to) {
        const auto start = static_cast<int>(std::round(from));
        const auto end = static_cast<int>(std::round(to));
        const auto width = end - start;
        if (width <= 0) {
            return;
        }

        auto sprite = sf::Sprite(*texture_);
        sprite.setTextureRect({
            {start, 0},
            {width, static_cast<int>(size_.y)}
        });
        sprite.setPosition({topLeft.x + static_cast<float>(start), topLeft.y});
        target.draw(sprite);
    };

    for (auto x = 0; x <= static_cast<int>(size_.x); ++x) {
        const auto worldX = position_.x - size_.x * 0.5f + static_cast<float>(x);
        const auto solid = x < static_cast<int>(size_.x) && !isInsideDestroyedHole(worldX);
        if (!solid) {
            drawSegment(segmentStart, static_cast<float>(x));
            segmentStart = static_cast<float>(x + 1);
        }
    }

    if (showHitbox) {
        renderHitboxes(target);
    }
}

bool WallBoss::isAlive(float logicalHeight) const {
    return position_.y - size_.y * 0.5f < logicalHeight + 24.f;
}

bool WallBoss::intersectsSolid(sf::FloatRect rect) const {
    const auto wallLeft = position_.x - size_.x * 0.5f;
    const auto wallTop = position_.y - size_.y * 0.5f;
    auto segmentLeft = wallLeft;
    auto segmentOpen = false;

    for (auto x = 0; x <= static_cast<int>(size_.x); ++x) {
        const auto worldX = wallLeft + static_cast<float>(x);
        const auto solid = x < static_cast<int>(size_.x) && !isInsideDestroyedHole(worldX);

        if (solid && !segmentOpen) {
            segmentLeft = worldX;
            segmentOpen = true;
        }

        if ((!solid || x == static_cast<int>(size_.x)) && segmentOpen) {
            const auto segmentRight = worldX;
            const auto solidBounds = sf::FloatRect{
                {segmentLeft, wallTop},
                {segmentRight - segmentLeft, size_.y}
            };
            if (intersects(solidBounds, rect)) {
                return true;
            }
            segmentOpen = false;
        }
    }

    return false;
}

bool WallBoss::isInsideDestroyedHole(float x) const {
    for (const auto& crystal : crystals_) {
        if (crystal.alive) {
            continue;
        }

        const auto centerX = position_.x + crystal.offset.x;
        if (x >= centerX - holeWidth_ * 0.5f && x < centerX + holeWidth_ * 0.5f) {
            return true;
        }
    }

    return false;
}

WallBoss::CrystalDamageResult WallBoss::damageCrystal(sf::FloatRect rect, float damage, int onlyInstanceId) {
    for (auto& crystal : crystals_) {
        if (!crystal.alive) {
            continue;
        }

        if (onlyInstanceId != 0 && crystal.instanceId != onlyInstanceId) {
            continue;
        }

        if (!intersects(crystalHitbox(crystal), rect)) {
            continue;
        }

        crystal.health = std::max(0.f, crystal.health - damage);
        if (crystal.health <= 0.f) {
            crystal.alive = false;
        }

        return {
            true,
            !crystal.alive,
            crystal.instanceId,
            crystal.patternId,
            crystalPosition(crystal)
        };
    }

    return {};
}

bool WallBoss::shouldFire(std::size_t crystalIndex) const {
    if (crystalIndex >= crystals_.size()) {
        return false;
    }

    const auto& crystal = crystals_[crystalIndex];
    return crystal.alive && crystal.patternId != "none" && crystal.fireTimer <= sf::Time::Zero;
}

void WallBoss::resetFireTimer(std::size_t crystalIndex, float intervalSeconds) {
    if (crystalIndex >= crystals_.size()) {
        return;
    }

    crystals_[crystalIndex].fireTimer = sf::seconds(intervalSeconds);
}

std::size_t WallBoss::crystalCount() const {
    return crystals_.size();
}

WallBoss::CrystalView WallBoss::crystal(std::size_t crystalIndex) const {
    if (crystalIndex >= crystals_.size()) {
        return {};
    }

    const auto& crystal = crystals_[crystalIndex];
    return {
        crystal.instanceId,
        crystal.id,
        crystal.patternId,
        crystalPosition(crystal),
        crystalHitbox(crystal),
        crystal.alive
    };
}

std::vector<WallBoss::CrystalView> WallBoss::crystals() const {
    auto views = std::vector<CrystalView>{};
    views.reserve(crystals_.size());
    for (auto index = std::size_t{0}; index < crystals_.size(); ++index) {
        views.push_back(crystal(index));
    }

    return views;
}

bool WallBoss::isCrystalOwnerAlive(int instanceId) const {
    auto alive = false;
    crystalPositionForOwner(instanceId, alive);
    return alive;
}

sf::Vector2f WallBoss::crystalPositionForOwner(int instanceId, bool& alive) const {
    for (const auto& crystal : crystals_) {
        if (crystal.instanceId == instanceId && crystal.alive) {
            alive = true;
            return crystalPosition(crystal);
        }
    }

    alive = false;
    return position_;
}

int WallBoss::contactDamage() const {
    return contactDamage_;
}

sf::FloatRect WallBoss::crystalHitbox(const Crystal& crystal) const {
    const auto center = crystalPosition(crystal);
    return {
        {
            center.x - crystal.hitboxSize.x * 0.5f,
            center.y - crystal.hitboxSize.y * 0.5f
        },
        crystal.hitboxSize
    };
}

sf::Vector2f WallBoss::crystalPosition(const Crystal& crystal) const {
    return position_ + crystal.offset;
}

void WallBoss::renderHitboxes(sf::RenderTarget& target) const {
    const auto wallLeft = position_.x - size_.x * 0.5f;
    const auto wallTop = position_.y - size_.y * 0.5f;
    auto segmentLeft = wallLeft;
    auto segmentOpen = false;

    for (auto x = 0; x <= static_cast<int>(size_.x); ++x) {
        const auto worldX = wallLeft + static_cast<float>(x);
        const auto solid = x < static_cast<int>(size_.x) && !isInsideDestroyedHole(worldX);

        if (solid && !segmentOpen) {
            segmentLeft = worldX;
            segmentOpen = true;
        }

        if ((!solid || x == static_cast<int>(size_.x)) && segmentOpen) {
            auto box = sf::RectangleShape({worldX - segmentLeft, size_.y});
            box.setPosition({
                std::round(segmentLeft),
                std::round(wallTop)
            });
            box.setFillColor(sf::Color::Transparent);
            box.setOutlineColor(sf::Color(60, 255, 80));
            box.setOutlineThickness(1.f);
            target.draw(box);
            segmentOpen = false;
        }
    }

    for (const auto& crystal : crystals_) {
        if (!crystal.alive) {
            continue;
        }

        const auto bounds = crystalHitbox(crystal);
        auto box = sf::RectangleShape(bounds.size);
        box.setPosition({
            std::round(bounds.position.x),
            std::round(bounds.position.y)
        });
        box.setFillColor(sf::Color::Transparent);
        box.setOutlineColor(sf::Color(80, 220, 255));
        box.setOutlineThickness(1.f);
        target.draw(box);
    }
}
