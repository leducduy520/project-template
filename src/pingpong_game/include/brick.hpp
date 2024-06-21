#ifndef _BRICK_H
#define _BRICK_H

#include "constants.hpp"
#include "entity.hpp"
#include <deque>
#include <utility>

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

class wall : public std::deque<brick>, public entity
{
public:
    wall() = default;
    template <class T = brick>
    wall(T&& brick_deque) noexcept
    {
        swap(std::forward<T>(brick_deque));
    }
    void update() override;
    void draw(sf::RenderWindow &window) override;

    void init(float x, float y) override;
};

#endif // _BRICK_H
