#ifndef UTILS_HPP
#define UTILS_HPP

#include <SFML/System/Vector2.hpp>
#include <string>

namespace Utils {
    sf::Vector2f getMovement(float deltaTime);
    void log(const std::string& message);
}

#endif // UTILS_HPP
