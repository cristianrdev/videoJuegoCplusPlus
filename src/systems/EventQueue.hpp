#pragma once

#include <SFML/System/Vector2.hpp>

#include <string>
#include <variant>
#include <vector>

struct EnemyDestroyedEvent {
    std::string enemyId;
    std::string patternId;
    int enemyInstanceId;
    sf::Vector2f position;
};

struct EnemyHitEvent {
    sf::Vector2f position;
};

struct PlayerHitEvent {
    int damage;
    int remainingHealth;
};

struct PlayerDestroyedEvent {
};

struct ItemCarrierDestroyedEvent {
    std::string dropId;
    sf::Vector2f position;
};

struct PowerUpCollectedEvent {
    std::string powerUpId;
};

using GameEvent = std::variant<
    EnemyDestroyedEvent,
    EnemyHitEvent,
    PlayerHitEvent,
    PlayerDestroyedEvent,
    ItemCarrierDestroyedEvent,
    PowerUpCollectedEvent
>;

class EventQueue {
public:
    void publish(GameEvent event);
    std::vector<GameEvent> drain();
    bool empty() const;

private:
    std::vector<GameEvent> events_;
};
