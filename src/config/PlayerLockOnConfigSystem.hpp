#pragma once

#include <string>

struct PlayerLockOnConfig {
    float chargeSeconds{3.f};
    float fieldRange{120.f};
    float fieldAngleDegrees{70.f};
    int maxTargets{8};
    float missileSpeed{170.f};
    float missileTurnRate{7.f};
    float missileSideExitSeconds{0.18f};
    float missileSideExitSpeed{120.f};
    float missileDamage{4.f};
    float missileHitboxWidth{6.f};
    float missileHitboxHeight{4.f};
    float missileVisualLength{16.f};
    float missileVisualWidth{4.f};
    float missileCoreWidth{2.f};
    float missileGlowRadius{6.f};
};

class PlayerLockOnConfigSystem {
public:
    void loadFromFile(const std::string& path);
    const PlayerLockOnConfig& config() const;

private:
    PlayerLockOnConfig config_;
};
