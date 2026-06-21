#include "EnemyConfigSystem.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir configuracion de enemigos: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string matchString(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo string en configuracion de enemigo: " + field);
    }

    return match[1].str();
}

std::string matchString(const std::string& text, const std::string& field, const std::string& fallback) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return match[1].str();
}

int matchInt(const std::string& text, const std::string& field, int fallback) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return std::stoi(match[1].str());
}

float matchFloat(const std::string& text, const std::string& field, float fallback) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return std::stof(match[1].str());
}

bool matchBool(const std::string& text, const std::string& field, bool fallback) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(true|false)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return match[1].str() == "true";
}
}

void EnemyConfigSystem::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);
    const auto objectPattern = std::regex("\\{[^\\{\\}]*\"id\"[^\\{\\}]*\\}");

    configs_.clear();
    for (auto it = std::sregex_iterator(text.begin(), text.end(), objectPattern);
         it != std::sregex_iterator{};
         ++it) {
        const auto object = (*it)[0].str();
        auto config = EnemyConfig{};
        config.id = matchString(object, "id");
        config.texturePath = matchString(object, "texture");
        config.health = matchInt(object, "health", 3);
        config.contactDamage = matchInt(object, "contact_damage", 0);
        config.hitboxShape = matchString(object, "hitbox_shape", "square");
        config.hitboxSize = {
            matchFloat(object, "hitbox_width", 0.f),
            matchFloat(object, "hitbox_height", 0.f)
        };
        config.hitboxOffset = {
            matchFloat(object, "hitbox_offset_x", 0.f),
            matchFloat(object, "hitbox_offset_y", 0.f)
        };
        config.secondaryPattern = matchString(object, "secondary_pattern", "none");
        config.secondarySpawnOffset = {
            matchFloat(object, "secondary_spawn_offset_x", 0.f),
            matchFloat(object, "secondary_spawn_offset_y", 0.f)
        };
        config.blinkEnabled = matchBool(object, "blink_enabled", false);
        config.blinkHealthThreshold = matchInt(object, "blink_health_threshold", 0);
        configs_[config.id] = std::move(config);
    }

    if (configs_.empty()) {
        throw std::runtime_error("No se cargaron configuraciones de enemigos desde: " + path);
    }
}

int EnemyConfigSystem::healthFor(const std::string& enemyId) const {
    const auto it = configs_.find(enemyId);
    if (it == configs_.end()) {
        return 3;
    }

    return it->second.health;
}

int EnemyConfigSystem::contactDamageFor(const std::string& enemyId) const {
    const auto it = configs_.find(enemyId);
    if (it == configs_.end()) {
        return 0;
    }

    return it->second.contactDamage;
}

sf::Vector2f EnemyConfigSystem::hitboxSizeFor(const std::string& enemyId) const {
    const auto it = configs_.find(enemyId);
    if (it == configs_.end()) {
        return {0.f, 0.f};
    }

    return it->second.hitboxSize;
}

sf::Vector2f EnemyConfigSystem::hitboxOffsetFor(const std::string& enemyId) const {
    const auto it = configs_.find(enemyId);
    if (it == configs_.end()) {
        return {0.f, 0.f};
    }

    return it->second.hitboxOffset;
}

const std::string& EnemyConfigSystem::hitboxShapeFor(const std::string& enemyId) const {
    static const auto fallback = std::string{"square"};
    const auto it = configs_.find(enemyId);
    if (it == configs_.end()) {
        return fallback;
    }

    return it->second.hitboxShape;
}

bool EnemyConfigSystem::blinkEnabledFor(const std::string& enemyId) const {
    const auto it = configs_.find(enemyId);
    if (it == configs_.end()) {
        return false;
    }

    return it->second.blinkEnabled;
}

int EnemyConfigSystem::blinkHealthThresholdFor(const std::string& enemyId) const {
    const auto it = configs_.find(enemyId);
    if (it == configs_.end()) {
        return 0;
    }

    return it->second.blinkHealthThreshold;
}

const std::string& EnemyConfigSystem::secondaryPatternFor(const std::string& enemyId) const {
    static const auto fallback = std::string{"none"};
    const auto it = configs_.find(enemyId);
    if (it == configs_.end()) {
        return fallback;
    }

    return it->second.secondaryPattern;
}

sf::Vector2f EnemyConfigSystem::secondarySpawnOffsetFor(const std::string& enemyId) const {
    const auto it = configs_.find(enemyId);
    if (it == configs_.end()) {
        return {0.f, 0.f};
    }

    return it->second.secondarySpawnOffset;
}

const std::string& EnemyConfigSystem::texturePathFor(const std::string& enemyId) const {
    const auto it = configs_.find(enemyId);
    if (it == configs_.end()) {
        throw std::runtime_error("Configuracion de enemigo no encontrada: " + enemyId);
    }

    return it->second.texturePath;
}

std::vector<std::string> EnemyConfigSystem::enemyIds() const {
    auto ids = std::vector<std::string>{};
    ids.reserve(configs_.size());
    for (const auto& [id, config] : configs_) {
        ids.push_back(id);
    }

    return ids;
}
