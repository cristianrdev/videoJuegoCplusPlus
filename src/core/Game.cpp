#include "Game.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <initializer_list>
#include <sstream>
#include <stdexcept>

namespace {
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
    , debugText_(debugFont_)
    , playState_(assets_, {static_cast<float>(LogicalWidth), static_cast<float>(LogicalHeight)}) {
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

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                window_.close();
            } else if (keyPressed->code == sf::Keyboard::Key::P) {
                togglePause();
            } else if (keyPressed->code == sf::Keyboard::Key::G) {
                playState_.toggleGodMode();
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
            } else if (keyPressed->code == sf::Keyboard::Key::Space ||
                       keyPressed->code == sf::Keyboard::Key::Z) {
                playState_.fireLaserNormal();
            }
        }

        if (const auto* buttonPressed = event->getIf<sf::Event::JoystickButtonPressed>()) {
            if (joystickPauseButton(buttonPressed->button)) {
                togglePause();
            }
        }
    }
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
    const auto scaleX = windowSize.x / LogicalWidth;
    const auto scaleY = windowSize.y / LogicalHeight;
    const auto scale = std::min(scaleX, scaleY);

    return std::clamp(scale, 1u, 4u);
}

void Game::togglePause() {
    paused_ = !paused_;
    if (paused_) {
        playState_.onPaused();
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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) ||
        joystickFirePressed()) {
        playState_.fireLaserNormal();
    }

    playState_.update(deltaTime);
}

void Game::render() {
    playState_.render(logicalTarget_);
    logicalTarget_.display();

    window_.clear(sf::Color::Black);
    window_.draw(presentationSprite_);
    renderGameOverOverlay();
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

void Game::renderGameOverOverlay() {
    if (!playState_.isGameOverVisible()) {
        return;
    }

    auto gameOverText = sf::Text(debugFont_);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(36);
    gameOverText.setFillColor(sf::Color(255, 90, 90));
    gameOverText.setOutlineColor(sf::Color(8, 12, 20));
    gameOverText.setOutlineThickness(2.f);

    const auto bounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin({
        bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y + bounds.size.y * 0.5f
    });
    gameOverText.setPosition({
        static_cast<float>(window_.getSize().x) * 0.5f,
        static_cast<float>(window_.getSize().y) * 0.5f
    });
    window_.draw(gameOverText);
}

void Game::renderDebugHud() {
    const auto spritePosition = presentationSprite_.getPosition();
    const auto leftPanelWidth = spritePosition.x;

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
         << playState_.stageTime().asSeconds()
         << " s\n\n"
         << "VIDA\n"
         << playState_.playerHealth()
         << "\n\nGOD\n"
         << (playState_.isGodModeEnabled() ? "ON" : "OFF")
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
         << "\n\nTECLAS\n"
         << "Esc salir\n"
         << "P pausa\n"
         << "Espacio disparo\n"
         << "Z disparo\n"
         << "G god mode\n"
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

    presentationSprite_.setTexture(logicalTarget_.getTexture(), true);
    presentationSprite_.setScale({integerScale, integerScale});

    const auto scaledWidth = static_cast<float>(LogicalWidth) * integerScale;
    const auto scaledHeight = static_cast<float>(LogicalHeight) * integerScale;
    presentationSprite_.setPosition({
        (static_cast<float>(windowSize.x) - scaledWidth) * 0.5f,
        (static_cast<float>(windowSize.y) - scaledHeight) * 0.5f
    });
}
