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

class entity : public Ientity
{
public:
    
    sf::FloatRect getGlobalbound() const noexcept;

    sf::Vector2f get_centre() const noexcept;

    float x() const noexcept;
    float y() const noexcept;
    float width() const noexcept;
    float height() const noexcept;

    float left() const noexcept;
    float right() const noexcept;
    float top() const noexcept;
    float bottom() const noexcept;

    virtual void destroy() noexcept;
    virtual bool is_destroyed() const noexcept;

    void set_position(const sf::Vector2f& pos) noexcept;

    virtual ~entity()
    {
    }
};


class moving_entity : public entity
{
protected:
    sf::Vector2f m_velocity;

public:

    virtual void move_up() noexcept = 0;
    virtual void move_down() noexcept = 0;
    virtual void move_left() noexcept = 0;
    virtual void move_right() noexcept = 0;
    virtual void print_info() const noexcept = 0;
    void set_velocity(const sf::Vector2f& vel) noexcept;
    sf::Vector2f get_velocity() const noexcept;
};

#endif // ENTITY_H
