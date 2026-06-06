#include "BackgroundElementDirector.hpp"

#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir orquestacion de fondo: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string matchString(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo string de elemento de fondo: " + field);
    }

    return match[1].str();
}

float matchFloat(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo numerico de elemento de fondo: " + field);
    }

    return std::stof(match[1].str());
}

int matchInt(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo entero de elemento de fondo: " + field);
    }

    return std::stoi(match[1].str());
}
}

void BackgroundElementDirector::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);
    const auto objectPattern = std::regex(
        "\\{[^\\{\\}]*\"time\"[^\\{\\}]*\"tileset\"[^\\{\\}]*\"tile\"[^\\{\\}]*\\}"
    );

    spawns_.clear();
    for (auto it = std::sregex_iterator(text.begin(), text.end(), objectPattern);
         it != std::sregex_iterator{};
         ++it) {
        const auto object = (*it)[0].str();
        auto spawn = SpawnEvent{};
        spawn.time = matchFloat(object, "time");
        spawn.tilesetId = matchString(object, "tileset");
        spawn.tileIndex = matchInt(object, "tile");
        spawn.x = matchFloat(object, "x");
        spawn.y = matchFloat(object, "y");
        spawn.speedY = matchFloat(object, "speed_y");
        spawns_.push_back(std::move(spawn));
    }

    std::sort(
        spawns_.begin(),
        spawns_.end(),
        [](const SpawnEvent& left, const SpawnEvent& right) {
            return left.time < right.time;
        }
    );

    reset();
}

std::vector<BackgroundElementDirector::SpawnEvent> BackgroundElementDirector::update(sf::Time deltaTime) {
    elapsed_ += deltaTime;
    auto ready = std::vector<SpawnEvent>{};

    while (nextSpawnIndex_ < spawns_.size() &&
           elapsed_.asSeconds() >= spawns_[nextSpawnIndex_].time) {
        ready.push_back(spawns_[nextSpawnIndex_]);
        ++nextSpawnIndex_;
    }

    return ready;
}

void BackgroundElementDirector::reset() {
    elapsed_ = sf::Time::Zero;
    nextSpawnIndex_ = 0;
}
