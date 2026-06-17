#pragma once

#include <string>

struct PlayerConfig {
    std::string shipTexture{"textures/player/player_ship_red_triangle_test.png"};
    std::string thrusterTexture{"textures/player/player_thruster_flame.png"};
    std::string laserTexture{"textures/player/player_laser_normal.png"};
    std::string muzzleFlashTexture{"textures/player/player_laser_muzzle_flash.png"};
    int health{3};
    int initialProjectileCount{1};
    float speed{120.f};
    float hitboxWidth{6.f};
    float hitboxHeight{6.f};
    float hitboxOffsetX{0.f};
    float hitboxOffsetY{0.f};
    float spriteRenderWidth{32.f};
    float spriteRenderHeight{32.f};
    float clampHalfSize{16.f};
    float laserSpawnOffsetY{-18.f};
    float fireCooldownSeconds{0.12f};
    float muzzleFlashSeconds{0.06f};
    float damageInvincibilitySeconds{1.f};
    float laserSpeed{260.f};
    int laserDamage{1};
    float thrusterAnimationSeconds{0.06f};
    int thrusterFrameWidth{8};
    int thrusterFrameHeight{12};
    int thrusterIdleHeight{9};
    int thrusterForwardHeight{12};
    int thrusterBackwardHeight{6};
    float thrusterLeftOffsetX{-3.f};
    float thrusterRightOffsetX{3.f};
    float thrusterOffsetY{13.f};
};

class PlayerConfigSystem {
public:
    void loadFromFile(const std::string& path);
    const PlayerConfig& config() const;

private:
    PlayerConfig config_;
};
