#include "StageCheckpointSystem.hpp"

#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
std::string readTextFile(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) {
        throw std::runtime_error("No se pudo abrir checkpoints: " + path);
    }

    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string matchString(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*\"([^\"]+)\"");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo string de checkpoint: " + field);
    }

    return match[1].str();
}

float matchFloat(const std::string& text, const std::string& field) {
    const auto pattern = std::regex("\"" + field + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    auto match = std::smatch{};
    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Falta campo numerico de checkpoint: " + field);
    }

    return std::stof(match[1].str());
}
}

void StageCheckpointSystem::loadFromFile(const std::string& path) {
    const auto text = readTextFile(path);
    const auto objectPattern = std::regex("\\{[^\\{\\}]*\"id\"[^\\{\\}]*\"time_seconds\"[^\\{\\}]*\\}");

    checkpoints_.clear();
    for (auto it = std::sregex_iterator(text.begin(), text.end(), objectPattern);
         it != std::sregex_iterator{};
         ++it) {
        const auto object = (*it)[0].str();
        checkpoints_.push_back({
            matchString(object, "id"),
            sf::seconds(matchFloat(object, "time_seconds"))
        });
    }

    std::sort(
        checkpoints_.begin(),
        checkpoints_.end(),
        [](const Checkpoint& left, const Checkpoint& right) {
            return left.time < right.time;
        }
    );
}

int StageCheckpointSystem::activeCheckpointIndex(sf::Time stageTime) const {
    auto active = 0;
    for (auto index = std::size_t{0}; index < checkpoints_.size(); ++index) {
        if (stageTime >= checkpoints_[index].time) {
            active = static_cast<int>(index) + 1;
        }
    }

    return active;
}

sf::Time StageCheckpointSystem::activeCheckpointTime(sf::Time stageTime) const {
    const auto index = activeCheckpointIndex(stageTime);
    if (index <= 0) {
        return sf::Time::Zero;
    }

    return checkpoints_[static_cast<std::size_t>(index - 1)].time;
}
