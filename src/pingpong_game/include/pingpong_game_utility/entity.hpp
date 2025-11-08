#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "SFML/Graphics.hpp"

class Ientity
{
private:
    sf::Sprite m_sprite;
    bool m_destroyed{false};

public:
    virtual ~Ientity() = default;
    virtual void update() = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void init(float px_x, float px_y) = 0;

    void destroy() noexcept
    {
        m_destroyed = true;
    }

    bool is_destroyed() const noexcept
    {
        return m_destroyed;
    }

    void set_scale(float, float);
    void set_position(const sf::Vector2f&);
    void move(const sf::Vector2f&);

    sf::Vector2f get_centre() const noexcept;
    float x() const noexcept;
    float y() const noexcept;
    float w() const noexcept;
    float h() const noexcept;
    float left() const noexcept;
    float top() const noexcept;
    float right() const noexcept;
    float bottom() const noexcept;

    Ientity() = default;
    Ientity(const Ientity& other) noexcept = default;
    Ientity& operator=(const Ientity& other) noexcept = default;
    Ientity(Ientity&& other) noexcept = default;
    Ientity& operator=(Ientity&& other) noexcept = default;

protected:
    void draw_sprite(sf::RenderWindow& window)
    {
        window.draw(m_sprite);
    };

    void set_origin_centre();
    void set_texture(const sf::Texture& texture);
};

class moving_entity : public Ientity
{
public:
    virtual void move_up() noexcept = 0;
    virtual void move_down() noexcept = 0;
    virtual void move_left() noexcept = 0;
    virtual void move_right() noexcept = 0;
    void set_velocity(const sf::Vector2f& vel) noexcept;
    sf::Vector2f get_velocity() const noexcept;
protected:
    sf::Vector2f m_velocity;
};

#endif // __ENTITY_H__
