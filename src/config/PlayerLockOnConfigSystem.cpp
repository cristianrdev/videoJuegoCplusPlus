#include "PlayerLockOnConfigSystem.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir configuracion lock-on: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

float matchFloat(const std::string& text, const std::string& field, float fallback) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return std::stof(match[1].str());
}

int matchInt(const std::string& text, const std::string& field, int fallback) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return std::stoi(match[1].str());
}
}

void PlayerLockOnConfigSystem::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);
    config_.chargeSeconds = matchFloat(text, "charge_seconds", config_.chargeSeconds);
    config_.fieldRange = matchFloat(text, "field_range", config_.fieldRange);
    config_.fieldAngleDegrees = matchFloat(text, "field_angle_degrees", config_.fieldAngleDegrees);
    config_.maxTargets = matchInt(text, "max_targets", config_.maxTargets);
    config_.missileSpeed = matchFloat(text, "missile_speed", config_.missileSpeed);
    config_.missileTurnRate = matchFloat(text, "missile_turn_rate", config_.missileTurnRate);
    config_.missileSideExitSeconds = matchFloat(text, "missile_side_exit_seconds", config_.missileSideExitSeconds);
    config_.missileSideExitSpeed = matchFloat(text, "missile_side_exit_speed", config_.missileSideExitSpeed);
    config_.missileDamage = matchFloat(text, "missile_damage", config_.missileDamage);
    config_.missileHitboxWidth = matchFloat(text, "missile_hitbox_width", config_.missileHitboxWidth);
    config_.missileHitboxHeight = matchFloat(text, "missile_hitbox_height", config_.missileHitboxHeight);
    config_.missileVisualLength = matchFloat(text, "missile_visual_length", config_.missileVisualLength);
    config_.missileVisualWidth = matchFloat(text, "missile_visual_width", config_.missileVisualWidth);
    config_.missileCoreWidth = matchFloat(text, "missile_core_width", config_.missileCoreWidth);
    config_.missileGlowRadius = matchFloat(text, "missile_glow_radius", config_.missileGlowRadius);
}

const PlayerLockOnConfig& PlayerLockOnConfigSystem::config() const {
    return config_;
}
