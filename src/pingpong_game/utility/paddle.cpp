#include "paddle.hpp"
#include <iostream>
#include <spdlog/spdlog.h>

sf::Texture& paddle::get_texture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized) {

        Ientity::check_resource_integrity("steel.png");

        if (!texture.loadFromFile((constants::resouces_path / "steel.png").string())) {
            spdlog::error("Failed to load paddle texture from: {}", (constants::resouces_path / "steel.png").string());
        }
        initialized = true;
    }
    return texture;
}

paddle::paddle(float px_x, float px_y) : moving_entity()
{
    set_texture(get_texture());
    set_origin_centre();
    paddle::init(px_x, px_y);
}

void paddle::init(float px_x, float px_y) noexcept
{
    set_position({px_x, px_y});
    set_velocity({0, 0});
}

void paddle::update()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && left() > 0.0f) {
        move_left();
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && right() < constants::window_width) {
        move_right();
    }
    else {
        if (std::abs(m_velocity.x) < constants::paddle_decel / 2.0f) {
            m_velocity = {0.0f, 0.0f};
        }
        else {
            m_velocity.x += constants::paddle_decel / 2.0f * (std::signbit(m_velocity.x) ? 1.0F : -1.0F);
        }
    }
    move(m_velocity);
}

void paddle::draw(sf::RenderWindow& window)
{
    draw_sprite(window);
}

void paddle::move_up() noexcept
{
    (void)0;
}

void paddle::move_left() noexcept
{
    if (m_velocity.x <= -constants::paddle_speed) {
        m_velocity = {-constants::paddle_speed, 0.0f};
    }
    else {
        m_velocity = {m_velocity.x + -constants::paddle_accel / 2.0f, 0.0f};
    }
}

void paddle::move_right() noexcept
{
    if (m_velocity.x >= constants::paddle_speed) {
        m_velocity = {constants::paddle_speed, 0.0f};
    }
    else {
        m_velocity = {m_velocity.x + constants::paddle_accel / 2.0f, 0.0f};
    }
}

void paddle::move_down() noexcept
{
    (void)0;
}
