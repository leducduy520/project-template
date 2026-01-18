#include "background.hpp"
#include "constants.hpp"
#include "resource_integrity.hpp"
#include <iostream>
#include <spdlog/spdlog.h>

sf::Texture& background::get_texture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized) {
        
        if (!texture.loadFromFile((constants::resouces_path / "background.jpg").string())) {
            spdlog::error("Failed to load background texture from: {}",
                          (constants::resouces_path / "background.jpg").string());
        }
        initialized = true;
    }
    return texture;
}

background::background(float px_x, float px_y)
{
    background::init(px_x, px_y);
}

void background::init(float px_x, float px_y) noexcept
{
    set_texture(get_texture());
    set_position({px_x, px_y});
}

// Compute the background's new position
void background::update() noexcept
{
    // Nothing to do here
}

void background::draw(sf::RenderWindow& window)
{
    // Ask the window to draw the sprite for us
    draw_sprite(window);
}
