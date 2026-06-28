#include "Game.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <initializer_list>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace {
constexpr auto RespawnPauseSeconds = 1.0f;

int firstConnectedJoystick() {
    for (auto joystick = 0u; joystick < sf::Joystick::Count; ++joystick) {
        if (sf::Joystick::isConnected(joystick)) {
            return static_cast<int>(joystick);
        }
    }

    return -1;
}

bool joystickButtonPressedAny(std::initializer_list<unsigned int> buttons) {
    const auto joystick = firstConnectedJoystick();
    if (joystick < 0) {
        return false;
    }

    const auto id = static_cast<unsigned int>(joystick);
    for (const auto button : buttons) {
        if (button < sf::Joystick::getButtonCount(id) &&
            sf::Joystick::isButtonPressed(id, button)) {
            return true;
        }
    }

    return false;
}

bool joystickFirePressed() {
    return joystickButtonPressedAny({
        0,  // Xbox A / PlayStation Cross / many generic primary buttons
        1,  // Xbox B / PlayStation Circle
        2,  // Xbox X / PlayStation Square
        3,  // Xbox Y / PlayStation Triangle
        4,  // LB / L1
        5   // RB / R1
    });
}

bool joystickPauseButton(unsigned int button) {
    return button == 7 ||  // Xbox Menu / common Start mapping
        button == 8 ||     // common generic Start mapping
        button == 9;       // PlayStation Options on many mappings
}
}

Game::Game()
    : assets_("assets")
    , window_(sf::VideoMode::getDesktopMode(), "Shooter vertical", sf::State::Fullscreen)
    , logicalTarget_({LogicalWidth, LogicalHeight})
    , presentationSprite_(logicalTarget_.getTexture())
    , debugText_(debugFont_) {
    restartPlayState();
    applyFramePacingMode(framePacingMode_);
    logicalTarget_.setSmooth(false);

    if (!debugFont_.openFromFile("C:/Windows/Fonts/consola.ttf")) {
        throw std::runtime_error("No se pudo cargar fuente debug: C:/Windows/Fonts/consola.ttf");
    }

    debugText_.setCharacterSize(16);
    debugText_.setFillColor(sf::Color(190, 220, 230));

    presentationIntegerScale_ = largestFittingIntegerScale();
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

        if (event->is<sf::Event::Resized>()) {
            presentationIntegerScale_ = largestFittingIntegerScale();
            updatePresentationSprite();
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                window_.close();
            } else if (playState_ &&
                       playState_->isGameOverVisible() &&
                       (keyPressed->code == sf::Keyboard::Key::Enter ||
                        keyPressed->code == sf::Keyboard::Key::Space)) {
                startNewGame();
            } else if (keyPressed->code == sf::Keyboard::Key::P) {
                togglePause();
            } else if (keyPressed->code == sf::Keyboard::Key::G) {
                playState_->toggleGodMode();
            } else if (keyPressed->code == sf::Keyboard::Key::H) {
                playState_->togglePlayerHitbox();
            } else if (keyPressed->code == sf::Keyboard::Key::T) {
                tateMode_ = !tateMode_;
                presentationIntegerScale_ = largestFittingIntegerScale();
                updatePresentationSprite();
            } else if (keyPressed->code == sf::Keyboard::Key::Q) {
                applyFramePacingMode(FramePacingMode::VSync);
            } else if (keyPressed->code == sf::Keyboard::Key::W) {
                applyFramePacingMode(FramePacingMode::Uncapped);
            } else if (keyPressed->code == sf::Keyboard::Key::E) {
                applyFramePacingMode(FramePacingMode::Cap120);
            } else if (keyPressed->code == sf::Keyboard::Key::Num1) {
                presentationIntegerScale_ = 1;
                updatePresentationSprite();
            } else if (keyPressed->code == sf::Keyboard::Key::Num2) {
                presentationIntegerScale_ = 2;
                updatePresentationSprite();
            } else if (keyPressed->code == sf::Keyboard::Key::Num3) {
                presentationIntegerScale_ = 3;
                updatePresentationSprite();
            } else if (keyPressed->code == sf::Keyboard::Key::Num4) {
                presentationIntegerScale_ = 4;
                updatePresentationSprite();
            }
        }

        if (const auto* buttonPressed = event->getIf<sf::Event::JoystickButtonPressed>()) {
            if (joystickPauseButton(buttonPressed->button)) {
                togglePause();
            }
        }
    }
}

void Game::startNewGame() {
    playerLives_ = InitialPlayerLives;
    restartPlayState(sf::Time::Zero);
}

void Game::restartPlayState(sf::Time initialStageTime) {
    paused_ = false;
    deathHandled_ = false;
    respawnPending_ = false;
    respawnCountdown_ = sf::Time::Zero;
    pendingRespawnStageTime_ = initialStageTime;
    playState_ = std::make_unique<PlayState>(
        assets_,
        sf::Vector2f{
            static_cast<float>(LogicalWidth),
            static_cast<float>(LogicalHeight)
        },
        initialStageTime
    );
}

void Game::applyFramePacingMode(FramePacingMode mode) {
    framePacingMode_ = mode;

    switch (framePacingMode_) {
    case FramePacingMode::VSync:
        window_.setFramerateLimit(0);
        window_.setVerticalSyncEnabled(true);
        break;
    case FramePacingMode::Uncapped:
        window_.setVerticalSyncEnabled(false);
        window_.setFramerateLimit(0);
        break;
    case FramePacingMode::Cap120:
        window_.setVerticalSyncEnabled(false);
        window_.setFramerateLimit(120);
        break;
    }
}

const char* Game::framePacingLabel() const {
    switch (framePacingMode_) {
    case FramePacingMode::VSync:
        return "Q VSYNC";
    case FramePacingMode::Uncapped:
        return "W UNCAPPED";
    case FramePacingMode::Cap120:
        return "E CAP 120";
    }

    return "UNKNOWN";
}

unsigned int Game::largestFittingIntegerScale() const {
    const auto windowSize = window_.getSize();
    const auto presentedWidth = tateMode_ ? LogicalHeight : LogicalWidth;
    const auto presentedHeight = tateMode_ ? LogicalWidth : LogicalHeight;
    const auto scaleX = windowSize.x / presentedWidth;
    const auto scaleY = windowSize.y / presentedHeight;
    const auto scale = std::min(scaleX, scaleY);

    return std::clamp(scale, 1u, 4u);
}

void Game::togglePause() {
    paused_ = !paused_;
    if (paused_) {
        playState_->onPaused();
    }
}

void Game::update(sf::Time deltaTime) {
    const auto frameSeconds = deltaTime.asSeconds();
    if (frameSeconds > 0.f) {
        const auto instantFps = 1.f / frameSeconds;
        smoothedFps_ = smoothedFps_ == 0.f ? instantFps : smoothedFps_ * 0.90f + instantFps * 0.10f;
    }

    if (paused_) {
        return;
    }

    if (respawnPending_) {
        respawnCountdown_ -= deltaTime;
        if (respawnCountdown_ <= sf::Time::Zero) {
            restartPlayState(pendingRespawnStageTime_);
        }
        return;
    }

    playState_->setFireButtonPressed(
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) ||
        joystickFirePressed()
    );

    playState_->update(deltaTime);

    if (playState_->isPlayerDestroyed() && !deathHandled_) {
        deathHandled_ = true;
        playerLives_ = std::max(0, playerLives_ - 1);
        if (playerLives_ > 0) {
            pendingRespawnStageTime_ = playState_->activeCheckpointTime();
            respawnCountdown_ = sf::seconds(RespawnPauseSeconds);
            respawnPending_ = true;
            playState_->onPaused();
        } else {
            pendingRespawnStageTime_ = sf::Time::Zero;
        }
    }
}

void Game::render() {
    playState_->render(logicalTarget_);
    renderLifeRespawnOverlay(logicalTarget_);
    renderGameOverOverlay(logicalTarget_);
    logicalTarget_.display();

    window_.clear(sf::Color::Black);
    window_.draw(presentationSprite_);
    renderPixelGrid();
    renderPauseOverlay();
    renderDebugHud();
    window_.display();
}

void Game::renderPauseOverlay() {
    if (!paused_) {
        return;
    }

    auto pauseText = sf::Text(debugFont_);
    pauseText.setString("PAUSE");
    pauseText.setCharacterSize(36);
    pauseText.setFillColor(sf::Color(235, 245, 255));
    pauseText.setOutlineColor(sf::Color(8, 12, 20));
    pauseText.setOutlineThickness(2.f);

    const auto bounds = pauseText.getLocalBounds();
    pauseText.setOrigin({
        bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y + bounds.size.y * 0.5f
    });
    pauseText.setPosition({
        static_cast<float>(window_.getSize().x) * 0.5f,
        static_cast<float>(window_.getSize().y) * 0.5f
    });
    window_.draw(pauseText);
}

void Game::renderGameOverOverlay(sf::RenderTarget& target) {
    if (!playState_ || !playState_->isGameOverVisible()) {
        return;
    }

    auto gameOverText = sf::Text(debugFont_);
    gameOverText.setString("GAME OVER\nENTER / ESPACIO");
    gameOverText.setCharacterSize(24);
    gameOverText.setFillColor(sf::Color(255, 90, 90));
    gameOverText.setOutlineColor(sf::Color(8, 12, 20));
    gameOverText.setOutlineThickness(1.f);

    const auto bounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin({
        bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y + bounds.size.y * 0.5f
    });
    gameOverText.setPosition({
        static_cast<float>(LogicalWidth) * 0.5f,
        static_cast<float>(LogicalHeight) * 0.5f
    });
    target.draw(gameOverText);
}

void Game::renderLifeRespawnOverlay(sf::RenderTarget& target) {
    if (!respawnPending_) {
        return;
    }

    auto text = sf::Text(debugFont_);
    text.setString("Vidas restantes: " + std::to_string(playerLives_));
    text.setCharacterSize(18);
    text.setFillColor(sf::Color(235, 245, 255));
    text.setOutlineColor(sf::Color(8, 12, 20));
    text.setOutlineThickness(1.f);

    const auto bounds = text.getLocalBounds();
    text.setOrigin({
        bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y + bounds.size.y * 0.5f
    });
    text.setPosition({
        static_cast<float>(LogicalWidth) * 0.5f,
        static_cast<float>(LogicalHeight) * 0.5f
    });
    target.draw(text);
}

void Game::renderPixelGrid() {
    if (!playState_ || !playState_->isPlayerHitboxVisible()) {
        return;
    }

    const auto scale = presentationSprite_.getScale().x;
    if (scale < 3.f) {
        return;
    }

    const auto color = sf::Color(255, 255, 255, 54);

    if (tateMode_) {
        auto verticalLine = sf::RectangleShape({
            static_cast<float>(LogicalHeight) * scale,
            1.f
        });
        verticalLine.setFillColor(color);
        for (auto x = 0u; x <= LogicalWidth; ++x) {
            const auto start = presentedLogicalPoint(static_cast<float>(x), 0.f);
            verticalLine.setPosition({
                std::round(start.x - static_cast<float>(LogicalHeight) * scale),
                std::round(start.y)
            });
            window_.draw(verticalLine);
        }

        auto horizontalLine = sf::RectangleShape({
            1.f,
            static_cast<float>(LogicalWidth) * scale
        });
        horizontalLine.setFillColor(color);
        for (auto y = 0u; y <= LogicalHeight; ++y) {
            const auto start = presentedLogicalPoint(0.f, static_cast<float>(y));
            horizontalLine.setPosition({
                std::round(start.x),
                std::round(start.y)
            });
            window_.draw(horizontalLine);
        }
        return;
    }

    const auto bounds = presentationBounds();
    const auto width = static_cast<float>(LogicalWidth) * scale;
    const auto height = static_cast<float>(LogicalHeight) * scale;

    auto verticalLine = sf::RectangleShape({1.f, height});
    verticalLine.setFillColor(color);
    for (auto x = 0u; x <= LogicalWidth; ++x) {
        verticalLine.setPosition({
            std::round(bounds.position.x + static_cast<float>(x) * scale),
            std::round(bounds.position.y)
        });
        window_.draw(verticalLine);
    }

    auto horizontalLine = sf::RectangleShape({width, 1.f});
    horizontalLine.setFillColor(color);
    for (auto y = 0u; y <= LogicalHeight; ++y) {
        horizontalLine.setPosition({
            std::round(bounds.position.x),
            std::round(bounds.position.y + static_cast<float>(y) * scale)
        });
        window_.draw(horizontalLine);
    }
}

void Game::renderDebugHud() {
    const auto bounds = presentationBounds();
    const auto leftPanelWidth = bounds.position.x;

    if (leftPanelWidth < 96.f) {
        return;
    }

    auto text = std::ostringstream{};
    const auto scale = presentationSprite_.getScale();
    const auto scaledWidth = static_cast<unsigned int>(
        std::round(static_cast<float>(LogicalWidth) * scale.x)
    );
    const auto scaledHeight = static_cast<unsigned int>(
        std::round(static_cast<float>(LogicalHeight) * scale.y)
    );

    text << std::fixed << std::setprecision(2)
         << "TIME\n"
         << playState_->stageTime().asSeconds()
         << " s\n\n"
         << "HP\n"
         << playState_->playerHealth()
         << "\n\nNAVES\n"
         << playerLives_
         << "\n\nCHECKPOINT\n"
         << playState_->activeCheckpointIndex()
         << "\n\nGOD\n"
         << (playState_->isGodModeEnabled() ? "ON" : "OFF")
         << "\n\nHITBOX\n"
         << (playState_->isPlayerHitboxVisible() ? "ON" : "OFF")
         << "\n\n"
         << std::setprecision(1)
         << "FPS\n"
         << smoothedFps_
         << "\n\nMODE\n"
         << framePacingLabel()
         << "\n\nNATIVE\n"
         << LogicalWidth << "x" << LogicalHeight
         << "\n\nSCALED\n"
         << scaledWidth << "x" << scaledHeight
         << "\n\nSCALE\n"
         << std::setprecision(0)
         << scale.x << "x"
         << "\n\nTATE\n"
         << (tateMode_ ? "ON" : "OFF")
         << "\n\nTECLAS\n"
         << "Esc salir\n"
         << "P pausa\n"
         << "Enter reintentar\n"
         << "Espacio disparo\n"
         << "Espacio reintentar\n"
         << "Z disparo\n"
         << "T tate\n"
         << "G god mode\n"
         << "H hitbox\n"
         << "1 escala 1x\n"
         << "2 escala 2x\n"
         << "3 escala 3x\n"
         << "4 escala 4x\n"
         << "Q vsync\n"
         << "W sin limite\n"
         << "E limite 120\n"
         << "\nMODOS\n"
         << "Q estable\n"
         << "W menor lag\n"
         << "E balance";

    debugText_.setString(text.str());
    debugText_.setPosition({24.f, 24.f});
    window_.draw(debugText_);
}

void Game::updatePresentationSprite() {
    const auto windowSize = window_.getSize();
    const auto integerScale = static_cast<float>(presentationIntegerScale_);
    const auto presentedWidth = static_cast<float>(tateMode_ ? LogicalHeight : LogicalWidth);
    const auto presentedHeight = static_cast<float>(tateMode_ ? LogicalWidth : LogicalHeight);
    const auto scaledWidth = presentedWidth * integerScale;
    const auto scaledHeight = presentedHeight * integerScale;
    const auto left = (static_cast<float>(windowSize.x) - scaledWidth) * 0.5f;
    const auto top = (static_cast<float>(windowSize.y) - scaledHeight) * 0.5f;

    presentationSprite_.setTexture(logicalTarget_.getTexture(), true);
    presentationSprite_.setScale({integerScale, integerScale});
    presentationSprite_.setOrigin({0.f, 0.f});

    if (tateMode_) {
        presentationSprite_.setRotation(sf::degrees(90.f));
        presentationSprite_.setPosition({
            left + static_cast<float>(LogicalHeight) * integerScale,
            top
        });
        return;
    }

    presentationSprite_.setRotation(sf::degrees(0.f));
    presentationSprite_.setPosition({left, top});
}

sf::FloatRect Game::presentationBounds() const {
    return presentationSprite_.getGlobalBounds();
}

sf::Vector2f Game::presentedLogicalPoint(float x, float y) const {
    const auto scale = presentationSprite_.getScale().x;
    const auto bounds = presentationBounds();

    if (tateMode_) {
        return {
            bounds.position.x + (static_cast<float>(LogicalHeight) - y) * scale,
            bounds.position.y + x * scale
        };
    }

    return {
        bounds.position.x + x * scale,
        bounds.position.y + y * scale
    };
}
