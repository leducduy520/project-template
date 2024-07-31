#include "background.hpp"

sf::Texture &background::getTexture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized)
    {
        if (!texture.loadFromFile(constants::resoucesPath + "background.jpg"))
        {
            std::cerr << "Get texture failed\n";
        }
        initialized = true;
    }
    return texture;
}

background::background(float px_x, float px_y)
{
    m_sprite.setTexture(getTexture());
    background::init(px_x, px_y);
}

void background::init(float px_x, float px_y)
{
    m_sprite.setPosition(px_x, px_y);
}

// Compute the background's new position
void background::update()
{
    // Nothing to do here
}

void background::draw(sf::RenderWindow &window)
{
    // Ask the window to draw the sprite for us
    window.draw(m_sprite);
}
