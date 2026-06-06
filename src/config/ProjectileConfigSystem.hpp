#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class ProjectileConfigSystem {
public:
    void loadFromFile(const std::string& path);
    const std::string& texturePathFor(const std::string& projectileId) const;
    std::vector<std::string> projectileIds() const;
    bool hasProjectile(const std::string& projectileId) const;

private:
    struct ProjectileConfig {
        std::string id;
        std::string texturePath;
    };

    std::unordered_map<std::string, ProjectileConfig> configs_;
};
