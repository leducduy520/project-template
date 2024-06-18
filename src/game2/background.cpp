#include "background.hpp"

background::background(float x, float y) : entity() {
  // Load the texture
  texture.loadFromFile("resources/background.jpg");
  sprite.setTexture(texture);

  // Set the initial position and velocity of the background
  // Use (x, y) for the initial position of the background
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
