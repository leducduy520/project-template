#ifndef __PADDLE__H
#define __PADDLE__H

#include "constants.hpp"
#include "entity.hpp"

class paddle : public moving_entity
{
    static sf::Texture& getTexture();

public:
    paddle(float x, float y);
    paddle() = default;
    void init(float x, float y) override;
    void update() override;
    void draw(sf::RenderWindow &window) override;
    void move_up(const float ratio = 1.0f) noexcept override;
    void move_down(const float ratio = 1.0f) noexcept override;
    void move_left(const float ratio = 1.0f) noexcept override;
    void move_right(const float ratio = 1.0f) noexcept override;

    void print_info() const noexcept override;
};
#endif // __PADDLE__H