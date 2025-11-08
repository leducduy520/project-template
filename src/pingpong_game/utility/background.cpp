#include "background.hpp"
#include "constants.hpp"
#include <iostream>

sf::Texture& background::get_texture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized)
    {
        if (!texture.loadFromFile((constants::resouces_path / "background.jpg").string()))
        {
            std::cerr << "Get texture failed\n";
        }
        initialized = true;
    }
    return texture;
}

background::background(float px_x, float px_y)
{
    m_sprite.setTexture(get_texture());
    background::init(px_x, px_y);
}

void background::init(float px_x, float px_y) noexcept
{
    m_sprite.setPosition(px_x, px_y);
}

// Compute the background's new position
void background::update() noexcept
{
    // Nothing to do here
}

void background::draw(sf::RenderWindow& window)
{
    // Ask the window to draw the sprite for us
    window.draw(m_sprite);
}
