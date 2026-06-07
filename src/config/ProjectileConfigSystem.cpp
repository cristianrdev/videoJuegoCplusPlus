#include "ProjectileConfigSystem.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir configuracion de proyectiles: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string matchString(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo string en configuracion de proyectil: " + field);
    }

    return match[1].str();
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

void ProjectileConfigSystem::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);
    const auto objectPattern = std::regex("\\{[^\\{\\}]*\"id\"[^\\{\\}]*\\}");

    configs_.clear();
    for (auto it = std::sregex_iterator(text.begin(), text.end(), objectPattern);
         it != std::sregex_iterator{};
         ++it) {
        const auto object = (*it)[0].str();
        auto config = ProjectileConfig{};
        config.id = matchString(object, "id");
        config.texturePath = matchString(object, "texture");
        config.damage = matchIntOr(object, "damage", 1);
        configs_[config.id] = std::move(config);
    }

    if (configs_.empty()) {
        throw std::runtime_error("No se cargaron configuraciones de proyectiles desde: " + path);
    }
}

const std::string& ProjectileConfigSystem::texturePathFor(const std::string& projectileId) const {
    const auto it = configs_.find(projectileId);
    if (it == configs_.end()) {
        throw std::runtime_error("Configuracion de proyectil no encontrada: " + projectileId);
    }

    return it->second.texturePath;
}

int ProjectileConfigSystem::damageFor(const std::string& projectileId) const {
    const auto it = configs_.find(projectileId);
    if (it == configs_.end()) {
        throw std::runtime_error("Configuracion de proyectil no encontrada: " + projectileId);
    }

    return it->second.damage;
}

std::vector<std::string> ProjectileConfigSystem::projectileIds() const {
    auto ids = std::vector<std::string>{};
    ids.reserve(configs_.size());
    for (const auto& [id, config] : configs_) {
        ids.push_back(id);
    }

    return ids;
}

bool ProjectileConfigSystem::hasProjectile(const std::string& projectileId) const {
    return configs_.find(projectileId) != configs_.end();
}
