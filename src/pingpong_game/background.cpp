#include "background.hpp"

sf::Texture &background::getTexture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized) {
        if (!texture.loadFromFile(constants::resoucesPath + "background.jpg")) {
            std::cerr << "Get texture failed\n";
        }
        initialized = true;
    }
    return texture;
}

background::background(float x, float y) : entity()
{
  init(x, y);
}

void background::init(float x, float y)
{
    sprite.setTexture(getTexture());
    sprite.setPosition(x, y);
}

// Compute the background's new position
void background::update() {
  // Nothing to do here
}

void background::draw(sf::RenderWindow& window) {
  // Ask the window to draw the sprite for us
  window.draw(sprite);
}
