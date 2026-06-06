#include "EventQueue.hpp"

void EventQueue::publish(GameEvent event) {
    events_.push_back(std::move(event));
}

std::vector<GameEvent> EventQueue::drain() {
    auto events = std::move(events_);
    events_.clear();
    return events;
}

bool EventQueue::empty() const {
    return events_.empty();
}
