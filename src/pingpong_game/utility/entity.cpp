#include "entity.hpp"

void Ientity::set_texture(const sf::Texture& texture)
{
    m_sprite.setTexture(texture);
}

void Ientity::set_origin_centre()
{
    m_sprite.setOrigin(w() / 2.0f, h() / 2.0f);
}

void Ientity::set_scale(float scale_x, float scale_y)
{
    m_sprite.setScale(scale_x, scale_y);
}

void Ientity::set_position(const sf::Vector2f& pos)
{
    m_sprite.setPosition(pos);
}

void Ientity::move(const sf::Vector2f& velocity)
{
    m_sprite.move(velocity);
}

sf::Vector2f Ientity::get_centre() const noexcept
{
    auto bounds = m_sprite.getLocalBounds();
    return {bounds.width / 2.0f, bounds.height / 2.0f};
}

float Ientity::x() const noexcept
{
    return m_sprite.getPosition().x;
}

float Ientity::y() const noexcept
{
    return m_sprite.getPosition().y;
}

float Ientity::w() const noexcept
{
    return m_sprite.getLocalBounds().width;
}

float Ientity::h() const noexcept
{
    return m_sprite.getLocalBounds().height;
}

float Ientity::left() const noexcept
{
    return m_sprite.getGlobalBounds().left;
}

float Ientity::top() const noexcept
{
    return m_sprite.getGlobalBounds().top;
}

float Ientity::right() const noexcept
{
    return m_sprite.getGlobalBounds().left + m_sprite.getGlobalBounds().width;
}

float Ientity::bottom() const noexcept
{
    return m_sprite.getGlobalBounds().top + m_sprite.getGlobalBounds().height;
}

void moving_entity::set_velocity(const sf::Vector2f& vel) noexcept
{
    m_velocity = vel;
}

sf::Vector2f moving_entity::get_velocity() const noexcept
{
    return m_velocity;
}
