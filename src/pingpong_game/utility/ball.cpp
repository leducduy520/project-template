#include "ball.hpp"
#include "soundplayer.hpp"
#include <filesystem>
#include <thread>


sf::Texture& ball::getTexture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized)
    {
        if (!texture.loadFromFile(constants::resoucesPath + "ball.png"))
        {
            std::cerr << "Get texture failed\n";
        }
        initialized = true;
    }
    return texture;
}

ball::ball(float px_x, float px_y) : moving_entity(), m_scaleflag(false), m_strength(1)
{
    m_sprite.setTexture(getTexture());
    m_sprite.setOrigin(get_centre());
    ball::init(px_x, px_y);
}

ball::ball() : m_scaleflag(false), m_strength(1)
{
    m_sprite.setTexture(getTexture());
    m_sprite.setOrigin(get_centre());
}

void ball::init(float px_x, float px_y)
{
    m_sprite.setPosition(px_x, px_y);
    m_velocity = {0.0F, constants::ball_speed};
}

// Compute the ball's new position
void ball::update()
{

    if (x() - getGlobalbound().width / 2 <= 0 && m_velocity.x < 0)
    {
        m_velocity.x = -m_velocity.x;
        SoundPlayer::getInstance()->playSound(SoundPlayer::WALL_BOUNCE);
    }
    else if (x() + getGlobalbound().width / 2 >= constants::window_width && m_velocity.x > 0)
    {
        m_velocity.x = -m_velocity.x;
        SoundPlayer::getInstance()->playSound(SoundPlayer::WALL_BOUNCE);
    }

    if (y() - getGlobalbound().height / 2 <= 0 && m_velocity.y < 0)
    {
        m_velocity.y = -m_velocity.y;
        SoundPlayer::getInstance()->playSound(SoundPlayer::WALL_BOUNCE);
    }
    else if (y() + getGlobalbound().height / 2 >= constants::window_height)
    {
        destroy();
    }

    m_sprite.move(m_velocity);
}

void ball::draw(sf::RenderWindow& window)
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

void ball::scale(const int& n) noexcept
{
    if (!m_scaleflag)
    {
        m_scaleflag = true;
        m_scale = n;
        m_strength = 3;
        m_sprite.setScale(n * 1.0F, n * 1.0F);
    }
}

void ball::resetsize() noexcept
{
    std::this_thread::sleep_for(std::chrono::seconds(10));
    m_sprite.setScale(1.0F, 1.0F);
    m_scaleflag = false;
    m_strength = 1;
}

int ball::getStrength() noexcept
{
    return m_strength;
}
