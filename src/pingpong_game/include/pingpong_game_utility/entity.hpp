#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <SFML/Graphics.hpp>
#include <iostream>

struct FloatRect : public sf::FloatRect
{
    float left;
    float top;
    float width;
    float height;

    FloatRect(float px_left, float px_top, float px_width, float px_height)
        : sf::FloatRect({px_left, px_top}, {px_width, px_height}),
          left(this->position.x),
          top(this->position.y),
          width(this->size.x),
          height(this->size.y)
    {
    }

    FloatRect(const sf::FloatRect& rect)
        : sf::FloatRect(rect.position, rect.size),
          left(this->position.x),
          top(this->position.y),
          width(this->size.x),
          height(this->size.y)
    {
    }
};

class Ientity
{
protected:
    sf::Texture m_texture;
    sf::Sprite m_sprite = sf::Sprite(m_texture);
    bool m_destroyed{false};

public:
    virtual ~Ientity() = default;
    virtual void update() = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void init(float px_x, float px_y) = 0;

    Ientity() = default;
    Ientity(const Ientity& other) noexcept = default;
    Ientity& operator=(const Ientity& other) noexcept = default;
    Ientity(Ientity&& other) noexcept = default;
    Ientity& operator=(Ientity&& other) noexcept = default;
};

class entity : public Ientity
{
public:
    entity() = default;
    FloatRect get_global_bound() const noexcept;

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

#endif // __ENTITY_H__
