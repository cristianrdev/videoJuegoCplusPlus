#pragma once

#include <SFML/Graphics/Texture.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>

class AssetManager {
public:
    explicit AssetManager(std::filesystem::path basePath);

    const sf::Texture& loadTexture(const std::string& id, const std::filesystem::path& relativePath);
    const sf::Texture& getTexture(const std::string& id) const;

private:
    std::filesystem::path basePath_;
    std::unordered_map<std::string, sf::Texture> textures_;
};
