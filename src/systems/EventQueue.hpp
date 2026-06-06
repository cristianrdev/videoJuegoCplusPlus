#pragma once

#include <SFML/System/Vector2.hpp>

#include <string>
#include <variant>
#include <vector>

struct EnemyDestroyedEvent {
    std::string enemyId;
    sf::Vector2f position;
};

struct PlayerHitEvent {
    int damage;
    int remainingHealth;
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
    PlayerHitEvent,
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
