#pragma once

#include "AssetManager.hpp"
#include "PlayState.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>

class Game {
public:
    Game();

    void run();

private:
    enum class PresentationScaleMode {
        Native,
        IntegerFit
    };

    enum class FramePacingMode {
        VSync,
        Uncapped,
        Cap120
    };

    void processEvents();
    void togglePause();
    void applyFramePacingMode(FramePacingMode mode);
    const char* framePacingLabel() const;
    void update(sf::Time deltaTime);
    void render();
    void renderPauseOverlay();
    void renderDebugHud();
    void updatePresentationSprite();

    static constexpr unsigned int LogicalWidth = 240;
    static constexpr unsigned int LogicalHeight = 320;

    AssetManager assets_;
    sf::RenderWindow window_;
    sf::RenderTexture logicalTarget_;
    sf::Sprite presentationSprite_;
    sf::Clock clock_;
    sf::Font debugFont_;
    sf::Text debugText_;
    PresentationScaleMode presentationScaleMode_{PresentationScaleMode::IntegerFit};
    FramePacingMode framePacingMode_{FramePacingMode::VSync};
    bool paused_{false};
    float smoothedFps_{0.f};
    PlayState playState_;
};
