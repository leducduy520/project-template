#include "paddle.hpp"

sf::Texture &paddle::getTexture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized) {
        if (!texture.loadFromFile(constants::resoucesPath + "steel.png")) {
            std::cerr << "Get texture failed\n";
        }
        initialized = true;
    }
    return texture;
}

paddle::paddle(float x, float y) : moving_entity()
{
    m_sprite.setTexture(getTexture());
    m_sprite.setOrigin(get_centre());
    init(x, y);
}

void paddle::init(float x, float y)
{
    m_sprite.setScale(constants::paddlle_width / m_sprite.getLocalBounds().width,
                    constants::paddlle_height / m_sprite.getLocalBounds().height);
    m_sprite.setPosition(x, y);
    m_velocity = {0, 0};
}

void paddle::update()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && x() >= get_bounding_box().width / 2.0f)
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
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && x() <= constants::window_width - get_bounding_box().width / 2.0f)
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
    else
    {
        if (std::abs(m_velocity.x) < constants::paddle_decel / 2.0f)
        {
            m_velocity = {0.0f, 0.0f};
        }
        else
        {
            m_velocity.x += constants::paddle_decel / 2.0f * _fdsign(m_velocity.x) ? 1 : -1;
        }
    }
    m_sprite.move(m_velocity);
}

void paddle::draw(sf::RenderWindow &window)
{
    window.draw(m_sprite);
}

void paddle::move_up(const float ratio) noexcept
{
}

void paddle::move_left(const float ratio) noexcept
{
    m_velocity.x = -constants::paddle_speed * ratio;
}

void paddle::move_right(const float ratio) noexcept
{
    m_velocity.x = constants::paddle_speed * ratio;
}

void paddle::move_down(const float ratio) noexcept
{
}

void paddle::print_info() const noexcept
{
    std::cout << "paddle centre: " << get_centre().x << " " << get_centre().y << '\n';
    std::cout << "paddle position: " << x() << " " << y() << '\n';
    std::cout << "paddle bounding box: " << get_bounding_box().width << " " << get_bounding_box().height << '\n';
    std::cout << '\n';
}
