#include "ball.hpp"


ball::ball(float x, float y) : moving_entity()
{
    // Load the texture
    texture.loadFromFile("resources/ball.png");
    sprite.setTexture(texture);
    sprite.setOrigin(get_centre());

    sprite.setPosition(x, y);
    velocity = {constants::ball_speed, constants::ball_speed};
}

// Compute the ball's new position
void ball::update()
{
    sprite.move(velocity);

    if (x() < 0)
        velocity.x = -velocity.x;
    if (x() > constants::window_width)
        velocity.x = -velocity.x;

    if (y() < 0)
        velocity.y = -velocity.y;
    if (y() > constants::window_height)
        velocity.y = -velocity.y;
}

void ball::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

void ball::move_up() noexcept
{
    velocity.y = -constants::ball_speed;
}

void ball::move_left() noexcept
{
    velocity.x = -constants::ball_speed;
}

void ball::move_right() noexcept
{
    velocity.x = constants::ball_speed;
}

void ball::move_down() noexcept
{
    velocity.y = constants::ball_speed;
}

void ball::print_info() const noexcept
{
    // std::cout << "ball centre: " << get_centre().x << " " << get_centre().y << '\n';
    // std::cout << "ball position: " << x() << " " << y() << '\n';
}
