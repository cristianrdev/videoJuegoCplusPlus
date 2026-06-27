#pragma once

#include "WallBossConfigSystem.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>
#include <vector>

class WallBoss {
public:
    struct CrystalView {
        int instanceId{0};
        std::string id;
        std::string patternId;
        sf::Vector2f position{0.f, 0.f};
        sf::FloatRect hitbox{};
        bool alive{false};
    };

    struct CrystalDamageResult {
        bool hit{false};
        bool destroyed{false};
        int instanceId{0};
        std::string patternId;
        sf::Vector2f position{0.f, 0.f};
    };

    WallBoss(
        sf::Vector2f position,
        const sf::Texture& texture,
        const WallBossConfigSystem::WallBossConfig& config
    );

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target, bool showHitbox = false) const;

    bool isAlive(float logicalHeight) const;
    bool intersectsSolid(sf::FloatRect rect) const;
    CrystalDamageResult damageCrystal(sf::FloatRect rect, float damage, int onlyInstanceId = 0);

    bool shouldFire(std::size_t crystalIndex) const;
    void resetFireTimer(std::size_t crystalIndex, float intervalSeconds);
    std::size_t crystalCount() const;
    CrystalView crystal(std::size_t crystalIndex) const;
    std::vector<CrystalView> crystals() const;
    bool isCrystalOwnerAlive(int instanceId) const;
    sf::Vector2f crystalPositionForOwner(int instanceId, bool& alive) const;

    int contactDamage() const;

private:
    struct Crystal {
        int instanceId{0};
        std::string id;
        std::string patternId;
        sf::Vector2f offset{0.f, 0.f};
        sf::Vector2f hitboxSize{24.f, 24.f};
        float health{30.f};
        sf::Time fireTimer{sf::Time::Zero};
        bool alive{true};
    };

    sf::FloatRect crystalHitbox(const Crystal& crystal) const;
    sf::Vector2f crystalPosition(const Crystal& crystal) const;
    void renderHitboxes(sf::RenderTarget& target) const;
    bool isInsideDestroyedHole(float x) const;

    sf::Vector2f position_{0.f, 0.f};
    sf::Vector2f size_{240.f, 32.f};
    float speedY_{18.f};
    float holeWidth_{40.f};
    int contactDamage_{3};
    const sf::Texture* texture_{nullptr};
    std::vector<Crystal> crystals_;
};
