#include <iostream>
#include "ball.hpp"
#include "ThreadPoolGame.hpp"
#include "soundplayer.hpp"
#include "helper.hpp"
#include "constants.hpp"
#include <spdlog/spdlog.h>

using namespace std::literals;

sf::Texture& ball::get_texture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized) {
        if (!texture.loadFromFile((constants::resouces_path / "ball.png").string())) {
            spdlog::error("Failed to load ball texture from: {}", (constants::resouces_path / "ball.png").string());
        }
        initialized = true;
    }
    return texture;
}

ball::ball(float px_x, float px_y) : moving_entity(), m_strength(constants::ball_strength_lv1)
{
    set_texture(get_texture());
    set_origin_centre();
    ball::init(px_x, px_y);
}

ball::ball() : m_strength(constants::ball_strength_lv1)
{
    set_texture(get_texture());
    set_origin_centre();
}

void ball::init(float px_x, float px_y)
{
    set_scale(1.0F, 1.0F);
    set_position({px_x, px_y});
    set_velocity({0.0F, constants::ball_speed});
}

// Compute the ball's new position
void ball::update()
{
    {
        const bool touch_left = x() - (w() / 2) <= 0 && m_velocity.x < 0;
        const bool touch_right = x() + (w() / 2) >= constants::window_width && m_velocity.x > 0;

        if (touch_left || touch_right) {
            m_velocity.x = -m_velocity.x;
            SoundPlayer::playSoundEffect(SoundPlayer::SoundEffect_t::WALL_BOUNCE);
        }
    }

    {
        const bool touch_up = y() - (h() / 2) <= 0 && m_velocity.y < 0;
        const bool touch_down = y() + (h() / 2) >= constants::window_height;
        if (touch_up) {
            m_velocity.y = -m_velocity.y;
            SoundPlayer::playSoundEffect(SoundPlayer::SoundEffect_t::WALL_BOUNCE);
        }
        else if (touch_down) {
            destroy();
        }
    }

    move(m_velocity);
    publish(Ientity::get_type_name<ball>() + "/update", this);
}

void ball::draw(sf::RenderWindow& window)
{
    draw_sprite(window);
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

void ball::scale(const int& n) noexcept
{
    set_scale(static_cast<float>(n), static_cast<float>(n));
    set_origin_centre();
    m_strength.store(m_strength.load() * n);
}

void ball::move_down() noexcept
{
    m_velocity.y = fabs(m_velocity.y);
}

int ball::strength() noexcept
{
    return m_strength;
}

size_t ball::get_counting_text_id() const noexcept
{
    return m_counting_text_id;
}

void ball::destroy() noexcept
{
    if (is_destroyed()) {
        return;
    }
    // Publish destruction message before destroying
    try {
        publish(Ientity::get_type_name<ball>() + "/destroyed", this);
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to publish destruction message: {}", e.what());
    }
    moving_entity::destroy();
}

void ball::set_counting_text_id(size_t id) noexcept
{
    m_counting_text_id = id;
}
