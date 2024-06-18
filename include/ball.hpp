#ifndef BALL_H
#define BALL_H



#include "constants.hpp"
#include "entity.hpp"

// Class to represent the bouncing ball
// Inherits from moving_entity
class ball : public moving_entity
{
    // Private data members
    sf::Texture texture;

public:
    ball(float x, float y);

    // Implement the pure virtual functions
    void update() override;
    void draw(sf::RenderWindow &window) override;

    void move_up() noexcept override;
    void move_down() noexcept override;
    void move_left() noexcept override;
    void move_right() noexcept override;
    void print_info() const noexcept override;
};

#endif // BALL_H
