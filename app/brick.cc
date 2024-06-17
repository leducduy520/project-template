#include "brick.h"

// Initialize static data
sf::Texture brick::texture;

brick::brick(float x, float y) : entity()
{
    // Load the texture
    texture.loadFromFile("resources/brick.png");
    sprite.setTexture(texture);
    sprite.setScale(1, 0.3125);
    // Set the initial position and velocity of the brick
    // Use (x, y) for the initial position of the brick
    sprite.setPosition(x, y);
}

// Compute the brick's new position
void brick::update()
{
}

void brick::draw(sf::RenderWindow &window)
{
    // Ask the window to draw the sprite for us
    window.draw(sprite);
}