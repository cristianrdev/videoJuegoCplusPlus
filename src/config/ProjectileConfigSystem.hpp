#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class ProjectileConfigSystem {
public:
    void loadFromFile(const std::string& path);
    const std::string& texturePathFor(const std::string& projectileId) const;
    bool hasTexture(const std::string& projectileId) const;
    const std::string& visualTypeFor(const std::string& projectileId) const;
    float visualLengthFor(const std::string& projectileId) const;
    float visualWidthFor(const std::string& projectileId) const;
    float visualGrowSecondsFor(const std::string& projectileId) const;
    bool rotateToVelocityFor(const std::string& projectileId) const;
    int damageFor(const std::string& projectileId) const;
    std::vector<std::string> projectileIds() const;
    bool hasProjectile(const std::string& projectileId) const;

private:
    struct ProjectileConfig {
        std::string id;
        std::string texturePath;
        std::string visualType{"sprite"};
        float visualLength{4.f};
        float visualWidth{4.f};
        float visualGrowSeconds{0.f};
        bool rotateToVelocity{false};
        int damage{1};
    };

    std::unordered_map<std::string, ProjectileConfig> configs_;
};
