#include "paddle.hpp"
#include <cmath>

sf::Texture& paddle::getTexture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized)
    {
        if (!texture.loadFromFile(constants::resoucesPath + "steel.png"))
        {
            std::cerr << "Get texture failed\n";
        }
        initialized = true;
    }
    return texture;
}

paddle::paddle(float px_x, float px_y) : moving_entity()
{
    m_sprite.setTexture(getTexture());
    m_sprite.setOrigin(get_centre());
    m_sprite.setScale(constants::paddlle_width / m_sprite.getLocalBounds().width,
                      constants::paddlle_height / m_sprite.getLocalBounds().height);
    paddle::init(px_x, px_y);
}

void paddle::init(float px_x, float px_y)
{
    m_sprite.setPosition(px_x, px_y);
    m_velocity = {0, 0};
}

void paddle::update()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && x() >= getGlobalbound().width / 2.0f)
    {
        move_left();
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
             x() <= constants::window_width - getGlobalbound().width / 2.0f)
    {
        move_right();
    }
    else
    {
        if (std::abs(m_velocity.x) < constants::paddle_decel / 2.0f)
        {
            m_velocity = {0.0f, 0.0f};
        }
        else
        {
            m_velocity.x += constants::paddle_decel / 2.0f * (std::signbit(m_velocity.x) ? 1.0F : -1.0F);
        }
    }
    m_sprite.move(m_velocity);
}

void paddle::draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}

void paddle::move_up() noexcept
{}

void paddle::move_left() noexcept
{
    if (m_velocity.x <= -constants::paddle_speed)
    {
        m_velocity = {-constants::paddle_speed, 0.0f};
    }
    else
    {
        m_velocity = {m_velocity.x - constants::paddle_accel / 2.0f, 0.0f};
    }
}

void paddle::move_right() noexcept
{
    if (m_velocity.x >= constants::paddle_speed)
    {
        m_velocity = {constants::paddle_speed, 0.0f};
    }
    else
    {
        m_velocity = {m_velocity.x + constants::paddle_accel / 2.0f, 0.0f};
    }
}

void paddle::move_down() noexcept
{}

void paddle::print_info() const noexcept
{
    std::cout << "paddle centre: " << get_centre().x << " " << get_centre().y << '\n';
    std::cout << "paddle position: " << x() << " " << y() << '\n';
    std::cout << "paddle bounding box: " << getGlobalbound().width << " " << getGlobalbound().height << '\n';
    std::cout << '\n';
}
