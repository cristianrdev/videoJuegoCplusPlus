#pragma once

#include "AssetManager.hpp"
#include "PlayState.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>

class Game {
public:
    Game();

    void run();

private:
    enum class FramePacingMode {
        VSync,
        Uncapped,
        Cap120
    };

    void processEvents();
    void startNewGame();
    void restartPlayState(sf::Time initialStageTime = sf::Time::Zero);
    void togglePause();
    void applyFramePacingMode(FramePacingMode mode);
    const char* framePacingLabel() const;
    unsigned int largestFittingIntegerScale() const;
    void update(sf::Time deltaTime);
    void render();
    void renderPauseOverlay();
    void renderGameOverOverlay(sf::RenderTarget& target);
    void renderLifeRespawnOverlay(sf::RenderTarget& target);
    void renderDebugHud();
    void renderPixelGrid();
    void updatePresentationSprite();
    sf::FloatRect presentationBounds() const;
    sf::Vector2f presentedLogicalPoint(float x, float y) const;

    static constexpr unsigned int LogicalWidth = 240;
    static constexpr unsigned int LogicalHeight = 320;
    static constexpr int InitialPlayerLives = 3;

    AssetManager assets_;
    sf::RenderWindow window_;
    sf::RenderTexture logicalTarget_;
    sf::Sprite presentationSprite_;
    sf::Clock clock_;
    sf::Font debugFont_;
    sf::Text debugText_;
    unsigned int presentationIntegerScale_{1};
    FramePacingMode framePacingMode_{FramePacingMode::VSync};
    bool paused_{false};
    bool tateMode_{false};
    bool deathHandled_{false};
    bool respawnPending_{false};
    int playerLives_{InitialPlayerLives};
    float smoothedFps_{0.f};
    sf::Time respawnCountdown_{sf::Time::Zero};
    sf::Time pendingRespawnStageTime_{sf::Time::Zero};
    std::unique_ptr<PlayState> playState_;
};
