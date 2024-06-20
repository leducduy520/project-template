#ifndef _BRICK_H
#define _BRICK_H

#include "constants.hpp"
#include "entity.hpp"
#include <deque>

class brick : public entity
{
    static sf::Texture& getTexture();
    static void releaseTexture();
public:
    brick(float x, float y);
    brick() = default;

    void init(float x, float y) override;

    // Implement the pure virtual functions
    void update() override;
    void draw(sf::RenderWindow &window) override;
    friend class wall;
};

class wall : public std::deque<brick>
{
public:
    void release();
    ~wall();
};

#endif // _BRICK_H
