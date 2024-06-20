#include "entity.hpp"

// Helper function to get the bounding box of a sprite
sf::FloatRect entity::get_bounding_box() const noexcept
{
    return sprite.getGlobalBounds();
}

// Helper function to get the centre of a sprite
sf::Vector2f entity::get_centre() const noexcept
{
    auto box = get_bounding_box();
    return {box.width / 2.0f, box.height / 2.0f};
}

// Helper functions to get the position of the sprite
float entity::x() const noexcept
{
    return sprite.getPosition().x;
}
float entity::y() const noexcept
{
    return sprite.getPosition().y;
}

float entity::width() const noexcept
{
    return sprite.getGlobalBounds().width;
}

float entity::height() const noexcept
{
    return sprite.getGlobalBounds().height;
}

void entity::destroy() noexcept
{
    destroyed = true;
}

bool entity::is_destroyed() const noexcept
{
    return destroyed;
}

float entity::left() const noexcept
{
    return sprite.getGlobalBounds().left;
}

float entity::right() const noexcept
{
    return sprite.getGlobalBounds().left + sprite.getGlobalBounds().width;
}

float entity::top() const noexcept
{
    return sprite.getGlobalBounds().top;
}

float entity::bottom() const noexcept
{
    return sprite.getGlobalBounds().top + sprite.getGlobalBounds().height;
}

void entity::set_position(const sf::Vector2f &pos) noexcept
{
    sprite.setPosition(pos);
}
