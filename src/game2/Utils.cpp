#include "Utils.hpp"
#include <iostream>

namespace Utils {
    sf::Vector2f getMovement(float deltaTime) {
        return sf::Vector2f(0.1f * deltaTime, 0.0f);
    }

    void log(const std::string& message) {
        std::cout << message << std::endl;
    }
}
