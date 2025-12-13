#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "SFML/Graphics.hpp"
#include "constants.hpp"
#include <list>
#include <functional>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <set>
#include <memory>

class entity_manager;
class IentityImpl;

class Ientity
{
private:
    friend class entity_manager;
    std::shared_ptr<IentityImpl> pImpl = {};

protected:
    // Private method to set the entity manager (only accessible by entity_manager)
    void set_manager(std::weak_ptr<entity_manager> manager) noexcept;

    // Private method to set the entity ID (only accessible by entity_manager)
    void set_id(size_t id) noexcept;

    // Subscribe to a topic (caches the topic for automatic cleanup)
    void subscribe(const std::string& topic);

    // Publish an event to a topic
    void publish(const std::string& topic, Ientity* entity);

    // Unsubscribe from a topic
    void unsubscribe(const std::string& topic);

    // Virtual method called when a message is received on a subscribed topic
    // Override this in derived classes to handle messages
    virtual void on_message(const std::string&, Ientity*)
    {
        (void)0;
    }

public:
    Ientity();
    Ientity(const Ientity& other) = delete;
    Ientity& operator=(const Ientity& other) = delete;
    virtual ~Ientity() noexcept;

    virtual void update() = 0;

    virtual void draw(sf::RenderWindow& window) = 0;

    virtual void init(float px_x, float px_y) = 0;

    virtual void destroy() noexcept;

    bool is_destroyed() const noexcept;

    size_t get_id() const noexcept;

    // Static method to get typeid name for a type
    template <typename T>
    static std::string get_type_name() noexcept
    {
        return std::string(typeid(T).name());
    }
};

class static_entity : public Ientity
{
    sf::Sprite m_sprite {constants::null_texture};

public:
    static_entity() = default;

    void update() override
    {
        (void)0;
    };

    void draw(sf::RenderWindow&) override
    {
        (void)0;
    };

    void init(float, float) override
    {
        (void)0;
    };

    void set_scale(float, float) noexcept;
    void set_position(const sf::Vector2f&) noexcept;
    void move(const sf::Vector2f&) noexcept;

    float x() const noexcept;
    float y() const noexcept;
    float w() const noexcept;
    float h() const noexcept;
    float left() const noexcept;
    float top() const noexcept;
    float right() const noexcept;
    float bottom() const noexcept;

protected:
    void draw_sprite(sf::RenderWindow& window) const noexcept
    {
        window.draw(m_sprite);
    }

    void set_origin_centre() noexcept;
    void set_texture(const sf::Texture& texture);
};

class moving_entity : public static_entity
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
