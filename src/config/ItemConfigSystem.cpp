#include "ItemConfigSystem.hpp"

#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir configuracion de items: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string matchString(const std::string& text, const std::string& field, const std::string& fallback = "") {
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

std::string arrayBody(const std::string& text, const std::string& field) {
    const auto startPattern = std::regex("\"" + field + "\"\\s*:\\s*\\[");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, startPattern)) {
        return {};
    }

    const auto begin = static_cast<std::size_t>(match.position() + match.length());
    auto depth = 1;
    for (auto i = begin; i < text.size(); ++i) {
        if (text[i] == '[') {
            ++depth;
        } else if (text[i] == ']') {
            --depth;
            if (depth == 0) {
                return text.substr(begin, i - begin);
            }
        }
    }

    return {};
}

std::vector<std::string> objectsInArray(const std::string& text, const std::string& field) {
    const auto body = arrayBody(text, field);
    const auto objectPattern = std::regex("\\{[^\\{\\}]*\\}");
    auto objects = std::vector<std::string>{};
    for (auto it = std::sregex_iterator(body.begin(), body.end(), objectPattern);
         it != std::sregex_iterator{};
         ++it) {
        objects.push_back((*it)[0].str());
    }

    return objects;
}
}

void ItemConfigSystem::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);
    carriers_.clear();
    powerUps_.clear();
    spawns_.clear();

    for (const auto& object : objectsInArray(text, "carriers")) {
        auto config = ItemCarrierConfig{};
        config.id = matchString(object, "id");
        config.texturePath = matchString(object, "texture");
        config.dropId = matchString(object, "drop");
        config.health = matchInt(object, "health", 5);
        config.speedY = matchFloat(object, "speed_y", 24.f);
        config.hitboxWidth = matchFloat(object, "hitbox_width", 15.f);
        config.hitboxHeight = matchFloat(object, "hitbox_height", 15.f);
        carriers_[config.id] = std::move(config);
    }

    for (const auto& object : objectsInArray(text, "power_ups")) {
        auto config = PowerUpConfig{};
        config.id = matchString(object, "id");
        config.texturePath = matchString(object, "texture");
        config.lifetimeSeconds = matchFloat(object, "lifetime_seconds", 30.f);
        config.flickerSeconds = matchFloat(object, "flicker_seconds", 10.f);
        config.amplitude = matchFloat(object, "amplitude", 28.f);
        config.frequency = matchFloat(object, "frequency", 1.4f);
        config.speedY = matchFloat(object, "speed_y", 8.f);
        config.hitboxWidth = matchFloat(object, "hitbox_width", 13.f);
        config.hitboxHeight = matchFloat(object, "hitbox_height", 13.f);
        config.transformTo = matchString(object, "transform_to", "none");
        config.transformIntervalSeconds = matchFloat(object, "transform_interval_seconds", 0.f);
        powerUps_[config.id] = std::move(config);
    }

    if (carriers_.empty() || powerUps_.empty()) {
        throw std::runtime_error("Configuracion de items incompleta: " + path);
    }
}

void ItemConfigSystem::loadStageSpawnsFromFile(const std::string& path) {
    const auto text = readTextFile(path);
    spawns_.clear();

    for (const auto& object : objectsInArray(text, "items")) {
        auto spawn = ItemSpawnConfig{};
        spawn.time = sf::seconds(matchFloat(object, "time", 0.f));
        spawn.itemId = matchString(object, "item");
        spawn.x = matchFloat(object, "x", 0.f);
        spawn.y = matchFloat(object, "y", 0.f);
        spawns_.push_back(std::move(spawn));
    }

    std::sort(
        spawns_.begin(),
        spawns_.end(),
        [](const ItemSpawnConfig& left, const ItemSpawnConfig& right) {
            return left.time < right.time;
        }
    );
}

const ItemCarrierConfig& ItemConfigSystem::carrierConfigFor(const std::string& itemId) const {
    const auto it = carriers_.find(itemId);
    if (it == carriers_.end()) {
        throw std::runtime_error("Item carrier no configurado: " + itemId);
    }

    return it->second;
}

const PowerUpConfig& ItemConfigSystem::powerUpConfigFor(const std::string& powerUpId) const {
    const auto it = powerUps_.find(powerUpId);
    if (it == powerUps_.end()) {
        throw std::runtime_error("Power up no configurado: " + powerUpId);
    }

    return it->second;
}

const std::vector<ItemSpawnConfig>& ItemConfigSystem::spawns() const {
    return spawns_;
}

std::vector<std::string> ItemConfigSystem::textureIds() const {
    auto ids = std::vector<std::string>{};
    ids.reserve(carriers_.size() + powerUps_.size());
    for (const auto& [id, config] : carriers_) {
        ids.push_back(id);
    }
    for (const auto& [id, config] : powerUps_) {
        ids.push_back(id);
    }

    return ids;
}

const std::string& ItemConfigSystem::texturePathFor(const std::string& itemId) const {
    if (const auto carrier = carriers_.find(itemId); carrier != carriers_.end()) {
        return carrier->second.texturePath;
    }
    if (const auto powerUp = powerUps_.find(itemId); powerUp != powerUps_.end()) {
        return powerUp->second.texturePath;
    }

    throw std::runtime_error("Textura de item no configurada: " + itemId);
}
