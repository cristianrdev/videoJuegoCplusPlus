#include "MovementPatternSystem.hpp"

#include <algorithm>
#include <array>
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

sf::Vector2f interpolateSegment(sf::Vector2f from, sf::Vector2f to, float localT) {
    return {
        from.x + (to.x - from.x) * localT,
        from.y + (to.y - from.y) * localT
    };
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
        pattern.dropY = matchFloat(object, "drop_y", 0.f);
        pattern.squareHalfWidth = matchFloat(object, "square_half_width", 40.f);
        pattern.squareHalfHeight = matchFloat(object, "square_half_height", 30.f);
        pattern.squarePeriod = matchFloat(object, "square_period", 4.f);
        pattern.squareCenterOffsetY = matchFloat(object, "square_center_offset_y", -70.f);
        pattern.screenWidth = matchFloat(object, "screen_width", 240.f);
        pattern.screenHeight = matchFloat(object, "screen_height", 320.f);
        pattern.marginX = matchFloat(object, "margin_x", 25.f);
        pattern.marginTop = matchFloat(object, "margin_top", 50.f);
        pattern.marginBottom = matchFloat(object, "margin_bottom", 50.f);
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
    return positionFor(patternId, startPosition, elapsed, startPosition);
}

sf::Vector2f MovementPatternSystem::positionFor(
    const std::string& patternId,
    sf::Vector2f startPosition,
    sf::Time elapsed,
    sf::Vector2f targetPosition
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

    if (pattern.type == "square_around_target") {
        const auto dropSpeed = std::max(1.f, pattern.velocityY);
        const auto dropDistance = std::max(0.f, pattern.dropY - startPosition.y);
        const auto dropTime = dropDistance / dropSpeed;

        if (t <= dropTime) {
            return {
                startPosition.x,
                startPosition.y + dropSpeed * t
            };
        }

        const auto center = sf::Vector2f{
            targetPosition.x,
            targetPosition.y + pattern.squareCenterOffsetY
        };
        const auto corners = std::array<sf::Vector2f, 4>{
            sf::Vector2f{center.x - pattern.squareHalfWidth, center.y - pattern.squareHalfHeight},
            sf::Vector2f{center.x + pattern.squareHalfWidth, center.y - pattern.squareHalfHeight},
            sf::Vector2f{center.x + pattern.squareHalfWidth, center.y + pattern.squareHalfHeight},
            sf::Vector2f{center.x - pattern.squareHalfWidth, center.y + pattern.squareHalfHeight}
        };
        const auto period = std::max(0.001f, pattern.squarePeriod);
        const auto normalized = std::fmod(t - dropTime, period) / period;
        const auto segment = std::min(3, static_cast<int>(normalized * 4.f));
        const auto localT = normalized * 4.f - static_cast<float>(segment);
        const auto& from = corners[static_cast<std::size_t>(segment)];
        const auto& to = corners[static_cast<std::size_t>((segment + 1) % 4)];

        return {
            from.x + (to.x - from.x) * localT,
            from.y + (to.y - from.y) * localT
        };
    }

    if (pattern.type == "screen_square_loop") {
        const auto dropSpeed = std::max(1.f, pattern.velocityY);
        const auto pathSpeed = std::max(1.f, std::abs(pattern.velocityX));
        const auto dropDistance = std::max(0.f, pattern.dropY - startPosition.y);
        const auto dropTime = dropDistance / dropSpeed;

        if (t <= dropTime) {
            return {
                startPosition.x,
                startPosition.y + dropSpeed * t
            };
        }

        const auto left = pattern.marginX;
        const auto right = pattern.screenWidth - pattern.marginX;
        const auto top = pattern.marginTop;
        const auto bottom = pattern.screenHeight - pattern.marginBottom;
        const auto entryStart = sf::Vector2f{startPosition.x, pattern.dropY};
        const auto entryEnd = sf::Vector2f{left, pattern.dropY};
        const auto entryTime = std::abs(entryStart.x - entryEnd.x) / pathSpeed;
        auto postDropTime = t - dropTime;

        if (postDropTime <= entryTime) {
            return interpolateSegment(entryStart, entryEnd, postDropTime / std::max(0.001f, entryTime));
        }

        postDropTime -= entryTime;
        const auto points = std::array<sf::Vector2f, 5>{
            sf::Vector2f{left, pattern.dropY},
            sf::Vector2f{left, top},
            sf::Vector2f{right, top},
            sf::Vector2f{right, bottom},
            sf::Vector2f{left, bottom}
        };
        const auto lengths = std::array<float, 5>{
            std::abs(points[1].y - points[0].y),
            std::abs(points[2].x - points[1].x),
            std::abs(points[3].y - points[2].y),
            std::abs(points[4].x - points[3].x),
            std::abs(points[4].y - points[0].y)
        };
        const auto loopTime =
            (lengths[0] + lengths[1] + lengths[2] + lengths[3] + lengths[4]) / pathSpeed;
        auto loopPosition = std::fmod(postDropTime, std::max(0.001f, loopTime)) * pathSpeed;

        for (auto i = std::size_t{0}; i < lengths.size(); ++i) {
            if (loopPosition <= lengths[i]) {
                const auto from = points[i];
                const auto to = i == points.size() - 1 ? points[0] : points[i + 1];
                return interpolateSegment(from, to, loopPosition / std::max(0.001f, lengths[i]));
            }
            loopPosition -= lengths[i];
        }

        return points[0];
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
