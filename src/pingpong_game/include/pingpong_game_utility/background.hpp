#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "entity.hpp"

class background : public entity
{
    // Private data members
    static sf::Texture& get_texture();

public:
    background(float px_x, float px_y);
    background() = default;

    void init(float px_x, float px_y) noexcept override;

    // Implement the pure virtual functions
    void update() noexcept override;
    void draw(sf::RenderWindow& window) override;
};

#endif // BACKGROUND_H
