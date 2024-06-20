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
    init(x, y);
}

void ball::init(float x, float y)
{
    // Load the texture
    sprite.setTexture(getTexture());
    sprite.setOrigin(get_centre());

    sprite.setPosition(x, y);
    velocity = {0, constants::ball_speed};
}

// Compute the ball's new position
void ball::update()
{
    sprite.move(velocity);

    if (x() - get_bounding_box().width / 2  < 0)
        velocity.x = -velocity.x;
    if (x() + get_bounding_box().width / 2 > constants::window_width)
        velocity.x = -velocity.x;

    if (y() - get_bounding_box().height < 0)
        velocity.y = -velocity.y;
    if (y() + get_bounding_box().height > constants::window_height)
        velocity.y = -velocity.y;
}

void ball::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

void ball::move_up(const float ratio) noexcept
{
    velocity.y = -constants::ball_speed * ratio;
}

void ball::move_left(const float ratio) noexcept
{
    velocity.x = -constants::ball_speed * ratio;
}

void ball::move_right(const float ratio) noexcept
{
    velocity.x = constants::ball_speed * ratio;
}

void ball::move_down(const float ratio) noexcept
{
    velocity.y = constants::ball_speed * ratio;
}

void ball::print_info() const noexcept
{
    // std::cout << "ball centre: " << get_centre().x << " " << get_centre().y << '\n';
    // std::cout << "ball position: " << x() << " " << y() << '\n';
}
