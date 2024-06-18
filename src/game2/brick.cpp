#include "brick.hpp"

// Define the static texture
sf::Texture brick::texture;

brick::brick(float x, float y) : entity()
{
    // Load the texture
    texture.loadFromFile("resources/brick.png");
    sprite.setTexture(texture);
    sprite.setScale(constants::brick_width / get_bounding_box().width,
                    constants::brick_height / get_bounding_box().height);
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