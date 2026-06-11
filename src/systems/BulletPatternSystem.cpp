#include "BulletPatternSystem.hpp"

#include <SFML/System/Vector2.hpp>

#include <algorithm>
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

std::string matchStringOr(const std::string& text, const std::string& field, const std::string& fallback) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return match[1].str();
}

bool matchBoolOr(const std::string& text, const std::string& field, bool fallback) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(true|false)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        return fallback;
    }

    return match[1].str() == "true";
}

bool matchBool(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(true|false)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo bool en patron: " + field);
    }

    return match[1].str() == "true";
}

float rotationDirectionFromText(const std::string& value) {
    if (value == "counterclockwise" || value == "anticlockwise" || value == "ccw" || value == "antihorario") {
        return -1.f;
    }

    return 1.f;
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
        pattern.bulletId = matchStringOr(object, "bullet", "default");
        pattern.laserId = matchStringOr(object, "laser", "default");
        pattern.type = matchStringOr(object, "type", "spread");
        pattern.fireInterval = matchFloat(object, "fire_interval");
        pattern.laserDuration = matchFloatOr(object, "laser_duration", 0.35f);
        pattern.bulletSpeed = matchFloatOr(object, "bullet_speed", 60.f);
        pattern.aimed = matchBoolOr(object, "aimed", false);
        pattern.angleOffsets = matchFloatArray(object, "angle_offsets");
        pattern.rings = matchIntOr(object, "rings", 1);
        pattern.bulletsPerRing = matchIntOr(object, "bullets_per_ring", 1);
        pattern.bulletsPerBurst = matchIntOr(object, "bullets_per_burst", 1);
        pattern.streams = matchIntOr(object, "streams", 1);
        pattern.speedStart = matchFloatOr(object, "speed_start", pattern.bulletSpeed);
        pattern.speedStep = matchFloatOr(object, "speed_step", 0.f);
        pattern.angleStep = matchFloatOr(object, "angle_step", 0.f);
        pattern.burstAngleSpacing = matchFloatOr(object, "burst_angle_spacing", 0.f);
        pattern.rotationPerShot = matchFloatOr(object, "rotation_per_shot", 0.f);
        pattern.angularVelocity = matchFloatOr(object, "angular_velocity", 0.f);
        pattern.bulletLifetime = matchFloatOr(object, "bullet_lifetime_seconds", 0.f);
        pattern.bulletFlickerBeforeDeath = matchFloatOr(object, "bullet_flicker_seconds", 0.f);
        pattern.rotationDirection = rotationDirectionFromText(matchStringOr(object, "rotation_direction", "clockwise"));
        pattern.spiralRadius = matchFloatOr(object, "spiral_radius", 24.f);
        pattern.spiralDescentSpeed = matchFloatOr(object, "spiral_descent_speed", 16.f);
        pattern.spiralRadiusExpansion = matchFloatOr(object, "spiral_radius_expansion", 0.f);
        pattern.clusterDuration = matchFloatOr(object, "cluster_duration_seconds", 0.f);
        pattern.bulletsPerSpiral = matchIntOr(object, "bullets_per_spiral", 24);
        pattern.spiralArms = matchIntOr(object, "spiral_arms", 1);
        pattern.fixedSpiralRadius = matchBoolOr(object, "fixed_spiral_radius", false);
        pattern.clearBulletsOnOwnerDestroyed = matchBoolOr(object, "clear_bullets_on_owner_destroyed", false);

        patterns_[pattern.id] = std::move(pattern);
    }

    if (patterns_.empty()) {
        throw std::runtime_error("No se cargaron patrones de bala desde: " + path);
    }
}

std::vector<EnemyBullet> BulletPatternSystem::spawn(
    const std::string& patternId,
    sf::Vector2f origin,
    sf::Vector2f target,
    const sf::Texture* bulletTexture,
    int bulletDamage,
    const std::string& visualType,
    sf::Vector2f visualSize,
    float visualGrowSeconds,
    int ownerInstanceId,
    bool rotateToVelocity
) {
    auto patternIt = patterns_.find(patternId);
    if (patternIt == patterns_.end()) {
        throw std::runtime_error("Patron de bala no encontrado: " + patternId);
    }

    auto& pattern = patternIt->second;
    const auto baseAngle = pattern.aimed ? aimedBaseAngleDegrees(origin, target) : 0.f;

    auto bullets = std::vector<EnemyBullet>{};

    if (pattern.type == "radial_burst") {
        bullets.reserve(static_cast<std::size_t>(pattern.rings * pattern.bulletsPerRing));
        const auto rotation = static_cast<float>(pattern.shotCounter) * pattern.rotationPerShot;

        for (auto ring = 0; ring < pattern.rings; ++ring) {
            const auto speed = pattern.speedStart + static_cast<float>(ring) * pattern.speedStep;
            const auto angularVelocity = degreesToRadians(pattern.angularVelocity);
            for (auto bullet = 0; bullet < pattern.bulletsPerRing; ++bullet) {
                const auto offset = rotation + static_cast<float>(bullet) * 360.f / static_cast<float>(pattern.bulletsPerRing);
                const auto angle = degreesToRadians(offset);
                const auto velocity = sf::Vector2f{std::sin(angle) * speed, std::cos(angle) * speed};
                if (pattern.angularVelocity != 0.f) {
                    bullets.emplace_back(
                        origin,
                        velocity,
                        bulletTexture,
                        bulletDamage,
                        visualType,
                        visualSize,
                        visualGrowSeconds,
                        ownerInstanceId,
                        rotateToVelocity,
                        pattern.bulletLifetime,
                        pattern.bulletFlickerBeforeDeath,
                        origin,
                        sf::Vector2f{0.f, 0.f},
                        0.f,
                        angle,
                        speed,
                        angularVelocity
                    );
                } else {
                    bullets.emplace_back(
                        origin,
                        velocity,
                        bulletTexture,
                        bulletDamage,
                        visualType,
                        visualSize,
                        visualGrowSeconds,
                        ownerInstanceId,
                        rotateToVelocity,
                        pattern.bulletLifetime,
                        pattern.bulletFlickerBeforeDeath
                    );
                }
            }
        }
    } else if (pattern.type == "rotating_clock") {
        bullets.reserve(static_cast<std::size_t>(std::max(1, pattern.bulletsPerBurst)));
        const auto rotation = std::fmod(
            static_cast<float>(pattern.shotCounter) * pattern.angleStep * pattern.rotationDirection,
            360.f
        );
        const auto burstCount = std::max(1, pattern.bulletsPerBurst);
        for (auto bullet = 0; bullet < burstCount; ++bullet) {
            const auto offset = rotation + static_cast<float>(bullet) * pattern.burstAngleSpacing;
            const auto angle = degreesToRadians(offset);
            const auto velocity = sf::Vector2f{
                std::sin(angle) * pattern.bulletSpeed,
                std::cos(angle) * pattern.bulletSpeed
            };
            bullets.emplace_back(
                origin,
                velocity,
                bulletTexture,
                bulletDamage,
                visualType,
                visualSize,
                visualGrowSeconds,
                ownerInstanceId,
                rotateToVelocity,
                pattern.bulletLifetime,
                pattern.bulletFlickerBeforeDeath
            );
        }
    } else if (pattern.type == "spiral_cluster") {
        const auto armCount = std::max(1, pattern.spiralArms);
        const auto bulletsPerArm = std::max(1, pattern.bulletsPerSpiral / armCount);
        const auto totalBullets = bulletsPerArm * armCount;
        bullets.reserve(static_cast<std::size_t>(totalBullets));
        const auto clusterRotation = static_cast<float>(pattern.shotCounter) * pattern.rotationPerShot * pattern.rotationDirection;
        const auto angularVelocity = degreesToRadians(pattern.angularVelocity * pattern.rotationDirection);
        const auto clusterLifetime = pattern.clusterDuration > 0.f ? pattern.clusterDuration : pattern.bulletLifetime;
        const auto flickerSeconds = pattern.bulletFlickerBeforeDeath;

        for (auto arm = 0; arm < armCount; ++arm) {
            const auto armOffset = static_cast<float>(arm) * 360.f / static_cast<float>(armCount);
            for (auto bullet = 0; bullet < bulletsPerArm; ++bullet) {
                const auto t = bulletsPerArm <= 1
                    ? 0.f
                    : static_cast<float>(bullet) / static_cast<float>(bulletsPerArm - 1);
                const auto radius = pattern.fixedSpiralRadius
                    ? pattern.spiralRadius
                    : pattern.spiralRadius * t;
                const auto angleDegrees =
                    clusterRotation +
                    armOffset +
                    static_cast<float>(bullet) * pattern.angleStep * pattern.rotationDirection;
                const auto angle = degreesToRadians(angleDegrees);
                const auto velocity = sf::Vector2f{
                    std::sin(angle) * pattern.bulletSpeed,
                    std::cos(angle) * pattern.bulletSpeed
                };
                bullets.emplace_back(
                    origin,
                    velocity,
                    bulletTexture,
                    bulletDamage,
                    visualType,
                    visualSize,
                    visualGrowSeconds,
                    ownerInstanceId,
                    rotateToVelocity,
                    clusterLifetime,
                    flickerSeconds,
                    origin,
                    sf::Vector2f{0.f, pattern.spiralDescentSpeed},
                    radius,
                    angle,
                    pattern.spiralRadiusExpansion,
                    angularVelocity
                );
            }
        }
    } else if (pattern.type == "rotating_stream") {
        bullets.reserve(static_cast<std::size_t>(pattern.streams));
        const auto rotation = static_cast<float>(pattern.shotCounter) * pattern.angleStep;
        for (auto stream = 0; stream < pattern.streams; ++stream) {
            const auto offset = rotation + static_cast<float>(stream) * 360.f / static_cast<float>(pattern.streams);
            const auto angle = degreesToRadians(offset);
            const auto velocity = sf::Vector2f{std::sin(angle) * pattern.bulletSpeed, std::cos(angle) * pattern.bulletSpeed};
            bullets.emplace_back(
                origin,
                velocity,
                bulletTexture,
                bulletDamage,
                visualType,
                visualSize,
                visualGrowSeconds,
                ownerInstanceId,
                rotateToVelocity,
                pattern.bulletLifetime,
                pattern.bulletFlickerBeforeDeath
            );
        }
    } else {
        auto offsets = pattern.angleOffsets;
        if (offsets.empty()) {
            offsets.push_back(0.f);
        }

        bullets.reserve(offsets.size());
        for (const auto offset : offsets) {
            const auto angle = degreesToRadians(baseAngle + offset);
            const auto velocity = sf::Vector2f{std::sin(angle) * pattern.bulletSpeed, std::cos(angle) * pattern.bulletSpeed};
            bullets.emplace_back(
                origin,
                velocity,
                bulletTexture,
                bulletDamage,
                visualType,
                visualSize,
                visualGrowSeconds,
                ownerInstanceId,
                rotateToVelocity,
                pattern.bulletLifetime,
                pattern.bulletFlickerBeforeDeath
            );
        }
    }

    ++pattern.shotCounter;
    return bullets;
}

float BulletPatternSystem::fireInterval(const std::string& patternId) const {
    return patternFor(patternId).fireInterval;
}

const std::string& BulletPatternSystem::bulletId(const std::string& patternId) const {
    return patternFor(patternId).bulletId;
}

const std::string& BulletPatternSystem::laserId(const std::string& patternId) const {
    return patternFor(patternId).laserId;
}

const std::string& BulletPatternSystem::patternType(const std::string& patternId) const {
    return patternFor(patternId).type;
}

float BulletPatternSystem::laserDuration(const std::string& patternId) const {
    return patternFor(patternId).laserDuration;
}

bool BulletPatternSystem::clearBulletsOnOwnerDestroyed(const std::string& patternId) const {
    return patternFor(patternId).clearBulletsOnOwnerDestroyed;
}

const BulletPatternSystem::Pattern& BulletPatternSystem::patternFor(const std::string& patternId) const {
    const auto it = patterns_.find(patternId);
    if (it == patterns_.end()) {
        throw std::runtime_error("Patron de bala no encontrado: " + patternId);
    }

    return it->second;
}
