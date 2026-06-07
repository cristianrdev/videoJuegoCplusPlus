#include "BackgroundElementConfigSystem.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir configuracion de elementos de fondo: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string matchString(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo string en elemento de fondo: " + field);
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

float matchFloat(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo numerico en elemento de fondo: " + field);
    }

    return std::stof(match[1].str());
}

int matchInt(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo entero en elemento de fondo: " + field);
    }

    return std::stoi(match[1].str());
}
}

void BackgroundElementConfigSystem::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);
    const auto objectPattern = std::regex("\\{[^\\{\\}]*\"tileset\"[^\\{\\}]*\"tile\"[^\\{\\}]*\\}");

    configs_.clear();
    for (auto it = std::sregex_iterator(text.begin(), text.end(), objectPattern);
         it != std::sregex_iterator{};
         ++it) {
        const auto object = (*it)[0].str();
        const auto tilesetId = matchString(object, "tileset");
        const auto tileIndex = matchInt(object, "tile");

        auto config = BackgroundElementConfig{};
        config.hitboxShape = matchStringOr(object, "hitbox_shape", "square");
        config.hitboxOffset = {
            matchFloat(object, "hitbox_offset_x"),
            matchFloat(object, "hitbox_offset_y")
        };
        config.hitboxSize = {
            matchFloat(object, "hitbox_width"),
            matchFloat(object, "hitbox_height")
        };

        configs_[keyFor(tilesetId, tileIndex)] = config;
    }

    if (configs_.empty()) {
        throw std::runtime_error("No se cargaron elementos de fondo desde: " + path);
    }
}

BackgroundElementConfig BackgroundElementConfigSystem::configFor(
    const std::string& tilesetId,
    int tileIndex
) const {
    const auto it = configs_.find(keyFor(tilesetId, tileIndex));
    if (it == configs_.end()) {
        return {};
    }

    return it->second;
}

std::string BackgroundElementConfigSystem::keyFor(const std::string& tilesetId, int tileIndex) {
    return tilesetId + "#" + std::to_string(tileIndex);
}
