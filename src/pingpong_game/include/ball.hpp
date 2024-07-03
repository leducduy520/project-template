#ifndef BALL_H
#define BALL_H



#include "constants.hpp"
#include "entity.hpp"
#include "SFML/Audio.hpp"

class ball : public moving_entity
{
    // Private data members
    static sf::Texture& getTexture();

public:
    ball(float px_x, float px_y);
    ball();

    void init(float px_x, float px_y) override;

    void update() override;
    void draw(sf::RenderWindow &window) override;

    void move_up() noexcept override;
    void move_down() noexcept override;
    void move_left() noexcept override;
    void move_right() noexcept override;
    void print_info() const noexcept override;
};

#endif // BALL_H
