#include "BulletPatternSystem.hpp"

#include <SFML/System/Vector2.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
constexpr auto Pi = 3.14159265358979323846f;

float degreesToRadians(float degrees) {
    return degrees * Pi / 180.f;
}

std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir configuracion de patrones: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string matchString(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo string en patron: " + field);
    }

    return match[1].str();
}

float matchFloat(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo numerico en patron: " + field);
    }

    return std::stof(match[1].str());
}

bool matchBool(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(true|false)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo bool en patron: " + field);
    }

    return match[1].str() == "true";
}

std::vector<float> matchFloatArray(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\\[([^\\]]*)\\]");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta arreglo numerico en patron: " + field);
    }

    auto values = std::vector<float>{};
    auto body = match[1].str();
    const auto numberPattern = std::regex("-?\\d+(?:\\.\\d+)?");
    for (auto it = std::sregex_iterator(body.begin(), body.end(), numberPattern);
         it != std::sregex_iterator{};
         ++it) {
        values.push_back(std::stof((*it)[0].str()));
    }

    return values;
}

float aimedBaseAngleDegrees(sf::Vector2f origin, sf::Vector2f target) {
    const auto delta = target - origin;
    return std::atan2(delta.x, delta.y) * 180.f / Pi;
}
}

void BulletPatternSystem::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);
    const auto objectPattern = std::regex("\\{[^\\{\\}]*\"id\"[^\\{\\}]*\\}");

    for (auto it = std::sregex_iterator(text.begin(), text.end(), objectPattern);
         it != std::sregex_iterator{};
         ++it) {
        const auto object = (*it)[0].str();
        auto pattern = Pattern{};
        pattern.id = matchString(object, "id");
        pattern.fireInterval = matchFloat(object, "fire_interval");
        pattern.bulletSpeed = matchFloat(object, "bullet_speed");
        pattern.aimed = matchBool(object, "aimed");
        pattern.angleOffsets = matchFloatArray(object, "angle_offsets");

        patterns_[pattern.id] = std::move(pattern);
    }

    if (patterns_.empty()) {
        throw std::runtime_error("No se cargaron patrones de bala desde: " + path);
    }
}

std::vector<EnemyBullet> BulletPatternSystem::spawn(
    const std::string& patternId,
    sf::Vector2f origin,
    sf::Vector2f target
) const {
    const auto& pattern = patternFor(patternId);
    const auto baseAngle = pattern.aimed ? aimedBaseAngleDegrees(origin, target) : 0.f;

    auto bullets = std::vector<EnemyBullet>{};
    bullets.reserve(pattern.angleOffsets.size());

    for (const auto offset : pattern.angleOffsets) {
        const auto angle = degreesToRadians(baseAngle + offset);
        const auto velocity = sf::Vector2f{
            std::sin(angle) * pattern.bulletSpeed,
            std::cos(angle) * pattern.bulletSpeed
        };
        bullets.emplace_back(origin, velocity);
    }

    return bullets;
}

float BulletPatternSystem::fireInterval(const std::string& patternId) const {
    return patternFor(patternId).fireInterval;
}

const BulletPatternSystem::Pattern& BulletPatternSystem::patternFor(const std::string& patternId) const {
    const auto it = patterns_.find(patternId);
    if (it == patterns_.end()) {
        throw std::runtime_error("Patron de bala no encontrado: " + patternId);
    }

    return it->second;
}
