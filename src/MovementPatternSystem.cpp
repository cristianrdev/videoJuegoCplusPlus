#include "MovementPatternSystem.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
constexpr auto Pi = 3.14159265358979323846f;

std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir configuracion de movimientos: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string matchString(const std::string& text, const std::string& field, const std::string& fallback = "") {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        if (!fallback.empty()) {
            return fallback;
        }
        throw std::runtime_error("Falta campo string en movimiento: " + field);
    }

    return match[1].str();
}

float matchFloat(const std::string& text, const std::string& field, float fallback = 0.f) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return std::stof(match[1].str());
}

std::vector<float> matchFloatArray(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\\[([^\\]]*)\\]");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return {};
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

sf::Vector2f interpolateWaypoints(const MovementPatternSystem::Pattern& pattern, sf::Vector2f startPosition, float time) {
    if (pattern.pointTimes.empty() ||
        pattern.pointTimes.size() != pattern.pointX.size() ||
        pattern.pointTimes.size() != pattern.pointY.size()) {
        return startPosition;
    }

    if (time <= pattern.pointTimes.front()) {
        return {pattern.pointX.front(), pattern.pointY.front()};
    }

    for (auto i = std::size_t{1}; i < pattern.pointTimes.size(); ++i) {
        if (time <= pattern.pointTimes[i]) {
            const auto startTime = pattern.pointTimes[i - 1];
            const auto endTime = pattern.pointTimes[i];
            const auto localT = (time - startTime) / std::max(0.001f, endTime - startTime);
            return {
                pattern.pointX[i - 1] + (pattern.pointX[i] - pattern.pointX[i - 1]) * localT,
                pattern.pointY[i - 1] + (pattern.pointY[i] - pattern.pointY[i - 1]) * localT
            };
        }
    }

    return {pattern.pointX.back(), pattern.pointY.back()};
}
}

void MovementPatternSystem::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);
    const auto objectPattern = std::regex("\\{[^\\{\\}]*\"id\"[^\\{\\}]*\\}");

    patterns_.clear();
    for (auto it = std::sregex_iterator(text.begin(), text.end(), objectPattern);
         it != std::sregex_iterator{};
         ++it) {
        const auto object = (*it)[0].str();
        auto pattern = Pattern{};
        pattern.id = matchString(object, "id");
        pattern.type = matchString(object, "type", "linear");
        pattern.velocityX = matchFloat(object, "velocity_x", 0.f);
        pattern.velocityY = matchFloat(object, "velocity_y", 0.f);
        pattern.amplitudeX = matchFloat(object, "amplitude_x", 0.f);
        pattern.frequency = matchFloat(object, "frequency", 1.f);
        pattern.phase = matchFloat(object, "phase", 0.f);
        pattern.pointTimes = matchFloatArray(object, "point_times");
        pattern.pointX = matchFloatArray(object, "point_x");
        pattern.pointY = matchFloatArray(object, "point_y");
        patterns_[pattern.id] = std::move(pattern);
    }

    if (patterns_.empty()) {
        throw std::runtime_error("No se cargaron patrones de movimiento desde: " + path);
    }
}

sf::Vector2f MovementPatternSystem::positionFor(
    const std::string& patternId,
    sf::Vector2f startPosition,
    sf::Time elapsed
) const {
    const auto& pattern = patternFor(patternId);
    const auto t = elapsed.asSeconds();

    if (pattern.type == "cosine") {
        return {
            startPosition.x + std::cos(t * pattern.frequency * 2.f * Pi + pattern.phase) * pattern.amplitudeX,
            startPosition.y + pattern.velocityY * t
        };
    }

    if (pattern.type == "waypoints") {
        return interpolateWaypoints(pattern, startPosition, t);
    }

    return {
        startPosition.x + pattern.velocityX * t,
        startPosition.y + pattern.velocityY * t
    };
}

const MovementPatternSystem::Pattern& MovementPatternSystem::patternFor(const std::string& patternId) const {
    const auto it = patterns_.find(patternId);
    if (it == patterns_.end()) {
        throw std::runtime_error("Patron de movimiento no encontrado: " + patternId);
    }

    return it->second;
}
