#include "paddle.h"

sf::Texture paddle::texture;

paddle::paddle(float x, float y) : moving_entity()
{
    texture.loadFromFile("resources/steel.png");
    sprite.setTexture(texture);
    sprite.setOrigin(get_centre());
    sprite.setScale(0.2f, 0.06f);
    sprite.setPosition(x, y - get_bounding_box().height / 2.0f);
    std::cout << get_bounding_box().left << " " << get_bounding_box().top << std::endl;
    std::cout << get_bounding_box().width << " " << get_bounding_box().height << std::endl;
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

void paddle::move_up() noexcept
{
}

void paddle::move_left() noexcept
{
    velocity.x = -constants::paddle_speed;
}

void paddle::move_right() noexcept
{
    velocity.x = constants::paddle_speed;
}

void paddle::move_down() noexcept
{
}

void paddle::print_info() const noexcept
{
    std::cout << "paddle centre: " << get_centre().x << " " << get_centre().y << '\n';
    std::cout << "paddle position: " << x() << " " << y() << '\n';
    std::cout << "paddle bounding box: " << get_bounding_box().width << " " << get_bounding_box().height << '\n';
    std::cout << '\n';
}


