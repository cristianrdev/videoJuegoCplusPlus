#include "Game.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <algorithm>
#include <cmath>

namespace {
constexpr auto WindowWidth = 1920u;
constexpr auto WindowHeight = 1080u;
}

Game::Game()
    : window_(sf::VideoMode({WindowWidth, WindowHeight}), "Shooter vertical")
    , logicalTarget_({LogicalWidth, LogicalHeight})
    , presentationSprite_(logicalTarget_.getTexture())
    , assets_("assets") {
    window_.setVerticalSyncEnabled(true);
    logicalTarget_.setSmooth(false);

    assets_.loadTexture("player_ship_center", "textures/player/player_ship_center.png");
    assets_.loadTexture("player_ship_left", "textures/player/player_ship_left.png");
    assets_.loadTexture("player_ship_right", "textures/player/player_ship_right.png");

    player_ = std::make_unique<Player>(
        assets_,
        sf::Vector2f{static_cast<float>(LogicalWidth), static_cast<float>(LogicalHeight)}
    );

    updatePresentationSprite();
}

void Game::run() {
    while (window_.isOpen()) {
        const auto deltaTime = clock_.restart();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    while (const auto event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_.close();
        }
    }
}

void Game::update(sf::Time deltaTime) {
    player_->update(deltaTime);
}

void Game::render() {
    logicalTarget_.clear(sf::Color(8, 12, 20));
    player_->render(logicalTarget_);
    logicalTarget_.display();

    window_.clear(sf::Color::Black);
    window_.draw(presentationSprite_);
    window_.display();
}

void Game::updatePresentationSprite() {
    const auto windowSize = window_.getSize();
    const auto scaleX = static_cast<float>(windowSize.x) / static_cast<float>(LogicalWidth);
    const auto scaleY = static_cast<float>(windowSize.y) / static_cast<float>(LogicalHeight);
    const auto integerScale = std::max(1.f, std::floor(std::min(scaleX, scaleY)));

    presentationSprite_.setTexture(logicalTarget_.getTexture(), true);
    presentationSprite_.setScale({integerScale, integerScale});

    const auto scaledWidth = static_cast<float>(LogicalWidth) * integerScale;
    const auto scaledHeight = static_cast<float>(LogicalHeight) * integerScale;
    presentationSprite_.setPosition({
        (static_cast<float>(windowSize.x) - scaledWidth) * 0.5f,
        (static_cast<float>(windowSize.y) - scaledHeight) * 0.5f
    });
}
