#include "AssetManager.hpp"

#include <stdexcept>

AssetManager::AssetManager(std::filesystem::path basePath)
    : basePath_(std::move(basePath)) {
}

const sf::Texture& AssetManager::loadTexture(const std::string& id, const std::filesystem::path& relativePath) {
    if (const auto existing = textures_.find(id); existing != textures_.end()) {
        return existing->second;
    }

    auto texture = sf::Texture{};
    const auto fullPath = basePath_ / relativePath;

    if (!texture.loadFromFile(fullPath.string())) {
        throw std::runtime_error("No se pudo cargar textura: " + fullPath.string());
    }

    texture.setSmooth(false);
    auto [it, inserted] = textures_.emplace(id, std::move(texture));
    return it->second;
}

const sf::Texture& AssetManager::getTexture(const std::string& id) const {
    const auto it = textures_.find(id);
    if (it == textures_.end()) {
        throw std::runtime_error("Textura no cargada: " + id);
    }

    return it->second;
}
