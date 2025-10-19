#include "ball.hpp"
#include "ThreadPoolGame.hpp"
#include "soundmanager.hpp"
#include "helper.hpp"
#include <filesystem>
#include <thread>

using namespace std::literals;

sf::Texture& ball::get_texture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized)
    {
        if (!texture.loadFromFile(constants::resouces_path + "ball.png"))
        {
            std::cerr << "Get texture failed\n";
        }
        initialized = true;
    }
    return texture;
}

ball::ball(float px_x, float px_y)
    : moving_entity(), m_text(utilities::texthelper::getFont(utilities::texthelper::CROSS_BOXED)), m_running(false), m_pause(false), m_strength(constants::ball_strength_lv1), m_scale_time(0)
{
    m_sprite.setTexture(get_texture());
    m_sprite.setOrigin(get_centre());
    ball::init(px_x, px_y);
}

ball::ball() 
    : moving_entity(), m_text(utilities::texthelper::getFont(utilities::texthelper::CROSS_BOXED)), m_running(false), m_pause(false), m_strength(constants::ball_strength_lv1), m_scale_time(0)
{
    m_sprite.setTexture(get_texture());
    m_sprite.setOrigin(get_centre());
}

ball::~ball()
{
    m_running.store(false);
    m_pause.store(false);
    if (m_result.valid())
    {
        m_result.get();
    }
}

void ball::init(float px_x, float px_y)
{
    using namespace utilities::texthelper;
    m_running.store(false);
    m_pause.store(false);
    m_strength.store(constants::ball_strength_lv1);
    m_scale_time.store(0);
    m_sprite.setScale({1.0F, 1.0F});
    m_sprite.setPosition({px_x, px_y});
    m_velocity = {0.0F, constants::ball_speed};
    m_text.setCharacterSize(14);
    m_text.setFillColor(sf::Color{0, 0, 255, 175});
    aligning::Aligning(&m_text, {left(), top(), width(), height() * 2}, aligning::BC);
}

// Compute the ball's new position
void ball::update()
{
    bool play_sound = false;
    {
        const bool touch_left = x() - get_global_bound().width / 2 <= 0 && m_velocity.x < 0;
        const bool touch_right = x() + get_global_bound().width / 2 >= constants::window_width && m_velocity.x > 0;

        if (touch_left || touch_right)
        {
            m_velocity.x = -m_velocity.x;
            play_sound = true;
        }
    }

    {
        const bool touch_up = y() - get_global_bound().height / 2 <= 0 && m_velocity.y < 0;
        const bool touch_down = y() + get_global_bound().height / 2 >= constants::window_height;
        if (touch_up)
        {
            m_velocity.y = -m_velocity.y;
            play_sound = true;
        }
        else if (touch_down)
        {
            destroy();
        }
    }

    if (!is_destroyed())
    {
        if (play_sound)
        {
            SoundManager::getInstance().playSoundEffect(SoundAdapter::get_sound_mode_id(SoundMode::WALL_BOUNCE));
        }
        m_sprite.move(m_velocity);
        m_text.move(m_velocity);
    }
}

void ball::draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
    window.draw(m_text);
}

void ball::move_up() noexcept
{
    m_velocity.y = -fabs(m_velocity.y);
}

void ball::move_left() noexcept
{
    m_velocity.x = -fabs(m_velocity.x);
}

void ball::move_right() noexcept
{
    m_velocity.x = fabs(m_velocity.x);
}

void ball::move_down() noexcept
{
    m_velocity.y = fabs(m_velocity.y);
}

void ball::print_info() const noexcept
{
    // std::cout << "ball centre: " << get_centre().x << " " << get_centre().y << '\n';
    // std::cout << "ball position: " << x() << " " << y() << '\n';
}

void ball::scale(const int& n) noexcept
{
    m_scale_time += 10000;
    if (!m_running.load())
    {
        m_running.store(true);
        m_strength.store(constants::ball_strength_lv2);
        const std::unique_lock<std::mutex> lock(m_mt);
        const auto rate = static_cast<float>(n);
        m_sprite.setScale({rate, rate});
        m_result = ThreadPool::getInstance().submit(0, &ball::reset_size, this);
    }
}

void ball::reset_size() noexcept
{
    auto start_time = std::chrono::steady_clock::now();
    auto current_time = start_time;

    while (std::chrono::duration_cast<std::chrono::milliseconds>((current_time - start_time)).count() <=
               m_scale_time.load() &&
           m_running.load())
    {
        if (m_pause.load())
        {
            start_time += 100ms;
        }
        else
        {
            auto time = std::chrono::duration_cast<std::chrono::seconds>((current_time - start_time)).count();
            auto total_time =
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(m_scale_time.load()))
                    .count();
            m_text.setString(utilities::texthelper::format_duration(total_time - time));
            if (total_time - time < 3)
            {
                m_text.setFillColor(sf::Color{255, 0, 0, 175});
            }
            utilities::texthelper::aligning::Aligning(&m_text,
                                                      {left(), top(), width(), height() * 2},
                                                      utilities::texthelper::aligning::BC);
            current_time = std::chrono::steady_clock::now();
        }
        std::this_thread::sleep_for(100ms);
    }

    m_running.store(false);
    m_strength.store(constants::ball_strength_lv1);
    m_scale_time.store(0);
    {
        const std::unique_lock<std::mutex> lock(m_mt);
        m_sprite.setScale({1.0F, 1.0F});
    }
    m_text.setString("");
    m_text.setFillColor(sf::Color{0, 0, 255, 175});
}

void ball::set_pause(const bool& pause)
{
    m_pause.store(pause);
}

void ball::stop()
{
    m_running.store(false);
    if (m_result.valid())
    {
        m_result.get();
    }
}

uint8_t ball::get_strength() noexcept
{
    return m_strength;
}

void ball::clone()
{
    using namespace std;
    using namespace utilities::random;
    clone_balls.emplace_front(sf::Vector2f{}, sf::Vector2f{});
    clone_balls.emplace_front(sf::Vector2f{}, sf::Vector2f{});

    const std::function<float(std::uniform_int_distribution<int>&, std::mt19937&)> rd_pos =
        [](std::uniform_int_distribution<int>& dist, std::mt19937& gen) { return static_cast<float>(dist(gen)); };
    const std::function<float(std::bernoulli_distribution&, std::mt19937&)> rd_vel =
        [](std::bernoulli_distribution& dist, std::mt19937& gen) { return dist(gen) ? 1.0F : -1.0F; };

    for (auto& a_ball : clone_balls)
    {
        const auto& master_pos = this->m_sprite.getPosition();
        const sf::Vector2f pos = {generator::getRandomInt(rd_pos) + master_pos.x,
                                  generator::getRandomInt(rd_pos) + master_pos.y};
        const sf::Vector2f vel = {generator::getRandomBoolean(rd_vel) * m_velocity.x,
                                  generator::getRandomBoolean(rd_vel) * m_velocity.y};
        a_ball = std::make_pair(pos, vel);
    }
}

std::pair<bool, time_t> ball::get_scale_status() const noexcept
{
    return std::make_pair(m_running.load(), m_scale_time.load());
}
