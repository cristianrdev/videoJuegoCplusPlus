#include "Game.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        auto game = Game{};
        game.run();
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << '\n';
        return 1;
    }

    return 0;
}
