#include "PlayerConfigSystem.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir configuracion de jugador: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string matchStringOr(const std::string& text, const std::string& field, const std::string& fallback) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return match[1].str();
}

float matchFloatOr(const std::string& text, const std::string& field, float fallback) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return std::stof(match[1].str());
}

int matchIntOr(const std::string& text, const std::string& field, int fallback) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return std::stoi(match[1].str());
}
}

void PlayerConfigSystem::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);

    config_.shipTexture = matchStringOr(text, "ship_texture", config_.shipTexture);
    config_.thrusterTexture = matchStringOr(text, "thruster_texture", config_.thrusterTexture);
    config_.laserTexture = matchStringOr(text, "laser_texture", config_.laserTexture);
    config_.muzzleFlashTexture = matchStringOr(text, "muzzle_flash_texture", config_.muzzleFlashTexture);
    config_.health = matchIntOr(text, "health", config_.health);
    config_.speed = matchFloatOr(text, "speed", config_.speed);
    config_.hitboxWidth = matchFloatOr(text, "hitbox_width", config_.hitboxWidth);
    config_.hitboxHeight = matchFloatOr(text, "hitbox_height", config_.hitboxHeight);
    config_.spriteRenderWidth = matchFloatOr(text, "sprite_render_width", config_.spriteRenderWidth);
    config_.spriteRenderHeight = matchFloatOr(text, "sprite_render_height", config_.spriteRenderHeight);
    config_.clampHalfSize = matchFloatOr(text, "clamp_half_size", config_.clampHalfSize);
    config_.laserSpawnOffsetY = matchFloatOr(text, "laser_spawn_offset_y", config_.laserSpawnOffsetY);
    config_.fireCooldownSeconds = matchFloatOr(text, "fire_cooldown_seconds", config_.fireCooldownSeconds);
    config_.muzzleFlashSeconds = matchFloatOr(text, "muzzle_flash_seconds", config_.muzzleFlashSeconds);
    config_.damageInvincibilitySeconds = matchFloatOr(text, "damage_invincibility_seconds", config_.damageInvincibilitySeconds);
    config_.laserSpeed = matchFloatOr(text, "laser_speed", config_.laserSpeed);
    config_.laserDamage = matchIntOr(text, "laser_damage", config_.laserDamage);
    config_.thrusterAnimationSeconds = matchFloatOr(text, "thruster_animation_seconds", config_.thrusterAnimationSeconds);
    config_.thrusterFrameWidth = matchIntOr(text, "thruster_frame_width", config_.thrusterFrameWidth);
    config_.thrusterFrameHeight = matchIntOr(text, "thruster_frame_height", config_.thrusterFrameHeight);
    config_.thrusterIdleHeight = matchIntOr(text, "thruster_idle_height", config_.thrusterIdleHeight);
    config_.thrusterForwardHeight = matchIntOr(text, "thruster_forward_height", config_.thrusterForwardHeight);
    config_.thrusterBackwardHeight = matchIntOr(text, "thruster_backward_height", config_.thrusterBackwardHeight);
    config_.thrusterLeftOffsetX = matchFloatOr(text, "thruster_left_offset_x", config_.thrusterLeftOffsetX);
    config_.thrusterRightOffsetX = matchFloatOr(text, "thruster_right_offset_x", config_.thrusterRightOffsetX);
    config_.thrusterOffsetY = matchFloatOr(text, "thruster_offset_y", config_.thrusterOffsetY);
}

const PlayerConfig& PlayerConfigSystem::config() const {
    return config_;
}
