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
    window_.setVerticalSyncEnabled(true);
    logicalTarget_.setSmooth(false);

    if (!debugFont_.openFromFile("C:/Windows/Fonts/consola.ttf")) {
        throw std::runtime_error("No se pudo cargar fuente debug: C:/Windows/Fonts/consola.ttf");
    }

    debugText_.setCharacterSize(16);
    debugText_.setFillColor(sf::Color(190, 220, 230));

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
            } else if (keyPressed->code == sf::Keyboard::Key::Num1) {
                presentationScaleMode_ = PresentationScaleMode::Native;
                updatePresentationSprite();
            } else if (keyPressed->code == sf::Keyboard::Key::Num2) {
                presentationScaleMode_ = PresentationScaleMode::IntegerFit;
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

void Game::renderDebugHud() {
    const auto spritePosition = presentationSprite_.getPosition();
    const auto leftPanelWidth = spritePosition.x;

    if (leftPanelWidth < 96.f) {
        return;
    }

    auto text = std::ostringstream{};
    text << std::fixed << std::setprecision(2)
         << "TIME\n"
         << playState_.stageTime().asSeconds()
         << " s\n\n"
         << std::setprecision(1)
         << "FPS\n"
         << smoothedFps_;

    debugText_.setString(text.str());
    debugText_.setPosition({24.f, 24.f});
    window_.draw(debugText_);
}

void Game::updatePresentationSprite() {
    const auto windowSize = window_.getSize();
    const auto scaleX = static_cast<float>(windowSize.x) / static_cast<float>(LogicalWidth);
    const auto scaleY = static_cast<float>(windowSize.y) / static_cast<float>(LogicalHeight);
    auto integerScale = 1.f;

    if (presentationScaleMode_ == PresentationScaleMode::IntegerFit) {
        integerScale = std::max(1.f, std::floor(std::min(scaleX, scaleY)));
    }

    presentationSprite_.setTexture(logicalTarget_.getTexture(), true);
    presentationSprite_.setScale({integerScale, integerScale});

    const auto scaledWidth = static_cast<float>(LogicalWidth) * integerScale;
    const auto scaledHeight = static_cast<float>(LogicalHeight) * integerScale;
    presentationSprite_.setPosition({
        (static_cast<float>(windowSize.x) - scaledWidth) * 0.5f,
        (static_cast<float>(windowSize.y) - scaledHeight) * 0.5f
    });
}
