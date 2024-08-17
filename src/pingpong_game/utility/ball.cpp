#include "ball.hpp"
#include "soundplayer.hpp"
#include <filesystem>
#include <thread>
#include "ThreadPoolGame.hpp"

using namespace std::literals;

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

ball::ball(float px_x, float px_y) : moving_entity(), m_scaleflag(false), m_pauseflag(false), m_strength(constants::ball_strength_lv1), m_scale_time(0)
{
    m_sprite.setTexture(getTexture());
    m_sprite.setOrigin(get_centre());
    ball::init(px_x, px_y);
}

ball::ball() : m_scaleflag(false), m_pauseflag(false), m_strength(constants::ball_strength_lv1), m_scale_time(0)
{
    m_sprite.setTexture(getTexture());
    m_sprite.setOrigin(get_centre());
}

ball::~ball()
{
    m_scaleflag.store(false);
    m_pauseflag.store(false);
}

void ball::init(float px_x, float px_y)
{
    m_sprite.setScale(1.0F, 1.0F);
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
    m_scale_time += 10000;
    if (!m_scaleflag.load())
    {
        m_scaleflag.store(true);
        m_strength.store(constants::ball_strength_lv2);
        std::unique_lock<std::mutex>(m_mt);
        m_sprite.setScale(n * 1.0F, n * 1.0F);
        ThreadPool::getInstance()->submit(0, &ball::reset_size, this);
    }
}

void ball::reset_size() noexcept
{
    auto start_time = std::chrono::steady_clock::now();
    auto current_time = start_time;

    while(std::chrono::duration_cast<std::chrono::milliseconds>((current_time - start_time)).count() <= m_scale_time.load() && m_scaleflag.load())
    {
        while(m_pauseflag.load()){
            std::this_thread::sleep_for(100ms);
            start_time += 100ms;
        }
        std::this_thread::sleep_for(100ms);
        current_time = std::chrono::steady_clock::now();
        //std::cout << "current_time: " << std::chrono::duration_cast<std::chrono::milliseconds>((current_time - start_time)).count() << "; duration: " << m_scale_time.load() << '\n';
    }

    if(!m_scaleflag.load())
    {
        return;
    }

    m_scaleflag.store(false);
    m_strength.store(constants::ball_strength_lv1);
    m_scale_time.store(0);
    {
        std::unique_lock<std::mutex>lock(m_mt);
        m_sprite.setScale(1.0F, 1.0F);
    }
}

void ball::set_pause(const bool& pause)
{
    m_pauseflag.store(pause);
}

void ball::stop()
{
    m_scaleflag.store(false);
}

int ball::get_strength() noexcept
{
    return m_strength;
}
