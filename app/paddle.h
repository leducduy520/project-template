#ifndef __PADDLE__H
#define __PADDLE__H
#include "constants.h"
#include "entity.h"

class paddle : public moving_entity
{
    static sf::Texture texture;

public:
    paddle(float x, float y);
    void update() override;
    void draw(sf::RenderWindow &window) override;
    void move_up() noexcept override;
    void move_down() noexcept override;
    void move_left() noexcept override;
    void move_right() noexcept override;

    void print_info() const noexcept override;
};
#endif // __PADDLE__H