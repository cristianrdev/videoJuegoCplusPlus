#include "StageDirector.hpp"

#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir stage: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string matchString(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo string de stage: " + field);
    }

    return match[1].str();
}

float matchFloat(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo numerico de stage: " + field);
    }

    return std::stof(match[1].str());
}
}

void StageDirector::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);
    const auto objectPattern = std::regex(
        "\\{[^\\{\\}]*\"time\"[^\\{\\}]*\"enemy\"[^\\{\\}]*\"pattern\"[^\\{\\}]*\\}"
    );

    spawns_.clear();
    for (auto it = std::sregex_iterator(text.begin(), text.end(), objectPattern);
         it != std::sregex_iterator{};
         ++it) {
        const auto object = (*it)[0].str();
        auto spawn = SpawnEvent{};
        spawn.time = matchFloat(object, "time");
        spawn.enemyId = matchString(object, "enemy");
        spawn.x = matchFloat(object, "x");
        spawn.y = matchFloat(object, "y");
        spawn.patternId = matchString(object, "pattern");
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

std::vector<StageDirector::SpawnEvent> StageDirector::update(sf::Time deltaTime) {
    elapsed_ += deltaTime;
    auto ready = std::vector<SpawnEvent>{};

    while (nextSpawnIndex_ < spawns_.size() &&
           elapsed_.asSeconds() >= spawns_[nextSpawnIndex_].time) {
        ready.push_back(spawns_[nextSpawnIndex_]);
        ++nextSpawnIndex_;
    }

    return ready;
}

void StageDirector::reset() {
    elapsed_ = sf::Time::Zero;
    nextSpawnIndex_ = 0;
}
