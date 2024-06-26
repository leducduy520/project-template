#include "ball.hpp"


sf::Texture &ball::getTexture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized) {
        if (!texture.loadFromFile(constants::resoucesPath + "ball.png")) {
            std::cerr << "Get texture failed\n";
        }
        initialized = true;
    }
    return texture;
}

ball::ball(float x, float y) : moving_entity()
{
    m_sprite.setTexture(getTexture());
    m_sprite.setOrigin(get_centre());
    init(x, y);
}

ball::ball()
{
    m_sprite.setTexture(getTexture());
    m_sprite.setOrigin(get_centre());
}

void ball::init(float x, float y)
{
    m_sprite.setPosition(x, y);
    m_velocity = {0, constants::ball_speed};
}

// Compute the ball's new position
void ball::update()
{

    if (x() - getGlobalbound().width / 2  <= 0 && m_velocity.x < 0)
        m_velocity.x = -m_velocity.x;
    if (x() + getGlobalbound().width / 2 >= constants::window_width && m_velocity.x > 0)
        m_velocity.x = -m_velocity.x;

    if (y() - getGlobalbound().height / 2 <= 0 && m_velocity.y < 0)
        m_velocity.y = -m_velocity.y;
    if (y() + getGlobalbound().height / 2 >= constants::window_height)
        destroy();

    m_sprite.move(m_velocity);
}

void ball::draw(sf::RenderWindow &window)
{
    window.draw(m_sprite);
}

void ball::move_up() noexcept
{
    m_velocity.y = -abs(m_velocity.y);
}

void ball::move_left() noexcept
{
    m_velocity.x = -abs(m_velocity.x);
}

void ball::move_right() noexcept
{
    m_velocity.x = abs(m_velocity.x);
}

void ball::move_down() noexcept
{
    m_velocity.y = abs(m_velocity.y);
}

void ball::print_info() const noexcept
{
    // std::cout << "ball centre: " << get_centre().x << " " << get_centre().y << '\n';
    // std::cout << "ball position: " << x() << " " << y() << '\n';
}
