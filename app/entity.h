#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <iostream>

// Abstract base class to represent any graphical entity on the screen
class entity
{
protected:
    sf::Sprite sprite;
    bool destroyed{false};
public:
    // Pure virtual functions
    // Concrete graphical entities must implement these functions
    // The update member function will compute the new position, appearance, etc of the object
    // The draw member function will cause the updated object to be displayed in the game window
    virtual void update() = 0;
    virtual void draw(sf::RenderWindow &window) = 0;

    // Helper function to get the bounding box of a sprite
    sf::FloatRect get_bounding_box() const noexcept;

    // Helper function to get the centre of a sprite
    sf::Vector2f get_centre() const noexcept;

    // Helper functions to get the position of the sprite
    float x() const noexcept;
    float y() const noexcept;

    virtual void destroy() noexcept;
    virtual bool is_destroyed() const noexcept;

    virtual float left()  const noexcept;
    virtual float right()  const noexcept;
    virtual float top()  const noexcept;
    virtual float bottom()  const noexcept;

    // Virtual destructor
    virtual ~entity()
    {
    }
};

// Abstract base class to represent graphical entities which can move around the screen
class moving_entity : public entity
{
protected:
    // SFML vector to store the object's velocity
    // This is the distance the ball moves between screen updates
    sf::Vector2f velocity;

public:
    // Helper functions inherited from parent class
    // float x();
    // float y();

    virtual void move_up() noexcept = 0;
    virtual void move_down() noexcept = 0;
    virtual void move_left() noexcept = 0;
    virtual void move_right() noexcept = 0;
    virtual void print_info() const noexcept = 0;

    // Pure virtual functions inherited from parent class
    // virtual void update() = 0;
    // virtual void draw(sf::RenderWindow& window) = 0;
    // etc
};

#endif // ENTITY_H
