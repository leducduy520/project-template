#include "paddle.hpp"

sf::Texture &paddle::getTexture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized) {
        if (!texture.loadFromFile("resources/steel.png")) {
            std::cerr << "Get texture failed\n";
        }
        initialized = true;
    }
    return texture;
}

paddle::paddle(float x, float y) : moving_entity()
{
    sprite.setTexture(getTexture());
    sprite.setOrigin(get_centre());
    sprite.setScale(constants::paddlle_width / get_bounding_box().width,
                    constants::paddlle_height / get_bounding_box().height);
    sprite.setPosition(x, y - get_bounding_box().height / 2.0f);
    velocity = {0, 0};
}

void paddle::update()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && x() >= get_bounding_box().width / 2.0f)
    {
        move_left();
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
             x() <= constants::window_width - get_bounding_box().width / 2.0f)
    {
        move_right();
    }
    else
    {
        velocity.x = 0;
    }
    sprite.move(velocity);
}

void paddle::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

void paddle::move_up(const float ratio) noexcept
{
}

void paddle::move_left(const float ratio) noexcept
{
    velocity.x = -constants::paddle_speed * ratio;
}

void paddle::move_right(const float ratio) noexcept
{
    velocity.x = constants::paddle_speed * ratio;
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
