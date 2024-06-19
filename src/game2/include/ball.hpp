#ifndef BALL_H
#define BALL_H



#include "constants.hpp"
#include "entity.hpp"

class ball : public moving_entity
{
    // Private data members
    static sf::Texture& getTexture();

public:
    ball(float x, float y);

    void update() override;
    void draw(sf::RenderWindow &window) override;

    void move_up(const float ratio = 1.0f) noexcept override;
    void move_down(const float ratio = 1.0f) noexcept override;
    void move_left(const float ratio = 1.0f) noexcept override;
    void move_right(const float ratio = 1.0f) noexcept override;
    void print_info() const noexcept override;
};

#endif // BALL_H
