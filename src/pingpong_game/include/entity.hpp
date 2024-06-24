#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <iostream>

class Ientity
{
protected:
    sf::Sprite m_sprite;
    bool m_destroyed{false};
    virtual ~Ientity() = default;

public:
    virtual void update() = 0;
    virtual void draw(sf::RenderWindow &window) = 0;
    virtual void init(float x, float y) = 0;
};

// Abstract base class to represent any graphical entity on the screen
class entity : public Ientity
{
public:
    
    sf::FloatRect getGlobalbound() const noexcept;

    // Helper function to get the centre of a sprite
    sf::Vector2f get_centre() const noexcept;

    // Helper functions to get the position of the sprite
    float x() const noexcept;
    float y() const noexcept;
    float width() const noexcept;
    float height() const noexcept;

    virtual float left() const noexcept;
    virtual float right() const noexcept;
    virtual float top() const noexcept;
    virtual float bottom() const noexcept;

    virtual void destroy() noexcept;
    virtual bool is_destroyed() const noexcept;


    void set_position(const sf::Vector2f& pos) noexcept;

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
    sf::Vector2f m_velocity;

public:
    // Helper functions inherited from parent class
    // float x();
    // float y();

    virtual void move_up() noexcept = 0;
    virtual void move_down() noexcept = 0;
    virtual void move_left() noexcept = 0;
    virtual void move_right() noexcept = 0;
    virtual void print_info() const noexcept = 0;
    void set_velocity(const sf::Vector2f& vel) noexcept;
    sf::Vector2f get_velocity() const noexcept;

    // Pure virtual functions inherited from parent class
    // virtual void update() = 0;
    // virtual void draw(sf::RenderWindow& window) = 0;
    // etc
};

#endif // ENTITY_H
