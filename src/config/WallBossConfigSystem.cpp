#include "WallBossConfigSystem.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir configuracion de wall bosses: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string matchString(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo string en wall boss: " + field);
    }

    return match[1].str();
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

std::string arrayBodyFor(const std::string& text, const std::string& field) {
    const auto fieldPosition = text.find("\"" + field + "\"");
    if (fieldPosition == std::string::npos) {
        return {};
    }

    const auto arrayStart = text.find('[', fieldPosition);
    if (arrayStart == std::string::npos) {
        return {};
    }

    auto depth = 0;
    for (auto index = arrayStart; index < text.size(); ++index) {
        if (text[index] == '[') {
            ++depth;
        } else if (text[index] == ']') {
            --depth;
            if (depth == 0) {
                return text.substr(arrayStart + 1, index - arrayStart - 1);
            }
        }
    }

    return {};
}

std::vector<std::string> objectsInArray(const std::string& text, const std::string& field) {
    const auto body = arrayBodyFor(text, field);
    auto objects = std::vector<std::string>{};
    auto depth = 0;
    auto objectStart = std::string::npos;

    for (auto index = std::size_t{0}; index < body.size(); ++index) {
        if (body[index] == '{') {
            if (depth == 0) {
                objectStart = index;
            }
            ++depth;
        } else if (body[index] == '}') {
            --depth;
            if (depth == 0 && objectStart != std::string::npos) {
                objects.push_back(body.substr(objectStart, index - objectStart + 1));
                objectStart = std::string::npos;
            }
        }
    }

    return objects;
}
}

void WallBossConfigSystem::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);

    configs_.clear();
    for (const auto& object : objectsInArray(text, "wall_bosses")) {
        auto config = WallBossConfig{};
        config.id = matchString(object, "id");
        config.texturePath = matchString(object, "texture");
        config.size = {
            matchFloatOr(object, "width", 240.f),
            matchFloatOr(object, "height", 32.f)
        };
        config.speedY = matchFloatOr(object, "speed_y", 18.f);
        config.contactDamage = matchIntOr(object, "contact_damage", 3);
        config.holeWidth = matchFloatOr(object, "hole_width", 40.f);
        config.textureOffsetX = matchFloatOr(object, "texture_offset_x", 0.f);

        for (const auto& crystalObject : objectsInArray(object, "crystals")) {
            auto crystal = CrystalConfig{};
            crystal.id = matchString(crystalObject, "id");
            crystal.offset = {
                matchFloatOr(crystalObject, "offset_x", 0.f),
                matchFloatOr(crystalObject, "offset_y", 0.f)
            };
            crystal.patternId = matchStringOr(crystalObject, "pattern", "none");
            crystal.health = matchFloatOr(crystalObject, "health", 30.f);
            crystal.hitboxSize = {
                matchFloatOr(crystalObject, "hitbox_width", 24.f),
                matchFloatOr(crystalObject, "hitbox_height", 24.f)
            };
            config.crystals.push_back(std::move(crystal));
        }

        configs_[config.id] = std::move(config);
    }

    if (configs_.empty()) {
        throw std::runtime_error("No se cargaron wall bosses desde: " + path);
    }
}

bool WallBossConfigSystem::hasWallBoss(const std::string& wallBossId) const {
    return configs_.find(wallBossId) != configs_.end();
}

const WallBossConfigSystem::WallBossConfig& WallBossConfigSystem::configFor(const std::string& wallBossId) const {
    const auto it = configs_.find(wallBossId);
    if (it == configs_.end()) {
        throw std::runtime_error("Wall boss no configurado: " + wallBossId);
    }

    return it->second;
}

const std::string& WallBossConfigSystem::texturePathFor(const std::string& wallBossId) const {
    return configFor(wallBossId).texturePath;
}

std::vector<std::string> WallBossConfigSystem::wallBossIds() const {
    auto ids = std::vector<std::string>{};
    ids.reserve(configs_.size());
    for (const auto& [id, config] : configs_) {
        ids.push_back(id);
    }

    return ids;
}
