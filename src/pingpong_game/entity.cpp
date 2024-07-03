#include "entity.hpp"

entity::entity() : Ientity()
{

}

// Helper function to get the bounding box of a sprite
sf::FloatRect entity::getGlobalbound() const noexcept
{
    return m_sprite.getGlobalBounds();
}

// Helper function to get the centre of a sprite
sf::Vector2f entity::get_centre() const noexcept
{
    auto box = getGlobalbound();
    return {box.width / 2.0f, box.height / 2.0f};
}

// Helper functions to get the position of the sprite
float entity::x() const noexcept
{
    return m_sprite.getPosition().x;
}
float entity::y() const noexcept
{
    return m_sprite.getPosition().y;
}

float entity::width() const noexcept
{
    return m_sprite.getLocalBounds().width;
}

float entity::height() const noexcept
{
    return m_sprite.getLocalBounds().height;
}

void entity::destroy() noexcept
{
    m_destroyed = true;
}

bool entity::is_destroyed() const noexcept
{
    return m_destroyed;
}

float entity::left() const noexcept
{
    return m_sprite.getGlobalBounds().left;
}

float entity::right() const noexcept
{
    return m_sprite.getGlobalBounds().left + m_sprite.getGlobalBounds().width;
}

float entity::top() const noexcept
{
    return m_sprite.getGlobalBounds().top;
}

float entity::bottom() const noexcept
{
    return m_sprite.getGlobalBounds().top + m_sprite.getGlobalBounds().height;
}

void entity::set_position(const sf::Vector2f &pos) noexcept
{
    m_sprite.setPosition(pos);
}

void moving_entity::set_velocity(const sf::Vector2f &vel) noexcept
{
    m_velocity = vel;
}

sf::Vector2f moving_entity::get_velocity() const noexcept
{
    return m_velocity;
}
