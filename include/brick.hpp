#ifndef _BRICK_H
#define _BRICK_H

#include "constants.hpp"
#include "entity.hpp"

// Class to represent the bouncing brick
// Inherits from moving_entity
class brick : public entity
{
    // Private data members
    static sf::Texture texture;
    
public:
    // Interface of the class
    // Constructor
    // Arguments are the initial coordinates of the centre of the brick
    // SFML uses the computer graphics convention
    // (0, 0) is the top left corner of the screen
    // x increases to the right
    // y increases downwards
    brick(float x, float y);

    // Implement the pure virtual functions
    void update() override;
    void draw(sf::RenderWindow &window) override;
};

#endif // _BRICK_H
