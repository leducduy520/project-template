#ifndef __PADDLE__H
#define __PADDLE__H

#include "constants.hpp"
#include "entity.hpp"

class paddle : public moving_entity
{
    static sf::Texture& getTexture();

public:
    paddle(float px_x, float px_y);
    paddle() = default;
    void init(float px_x, float px_y) override;
    void update() override;
    void draw(sf::RenderWindow &window) override;
    void move_up() noexcept override;
    void move_down() noexcept override;
    void move_left() noexcept override;
    void move_right() noexcept override;

    void print_info() const noexcept override;
};
#endif // __PADDLE__H