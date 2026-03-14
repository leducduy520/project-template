#include "entity.hpp"
#include "entitymanager.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

struct IentityImpl
{
    bool m_destroyed{false};
    size_t m_id{0};
    Ientity* m_owner{nullptr};
    std::weak_ptr<entity_manager> m_manager{};
    std::set<std::string, std::less<>> m_subscribed_topics = {}; // Cache of subscribed topics

    // Private method to set the entity manager (only accessible by entity_manager)
    void set_manager(std::weak_ptr<entity_manager> manager) noexcept
    {
        m_manager = manager;
        if (m_manager.expired()) {
            spdlog::warn("Manager is expired after set_manager");
        }
    }

    // Private method to set the entity ID (only accessible by entity_manager)
    void set_id(size_t id) noexcept
    {
        m_id = id;
    }

    // Subscribe to a topic (caches the topic for automatic cleanup)
    void subscribe(const std::string& topic)
    {
        if (m_manager.expired()) return;
        auto pmanager = m_manager.lock();
        if (!topic.empty() && pmanager != nullptr && m_subscribed_topics.find(topic) == m_subscribed_topics.end()) {
            pmanager->subscribe(topic, m_owner);
            m_subscribed_topics.insert(topic); // Cache the topic
        }
    }

    // Publish an event to a topic
    void publish(const std::string& topic, Ientity* entity) const
    {
        if (m_manager.expired()) return;
        m_manager.lock()->publish(topic, entity);
    }

    // Unsubscribe from a topic
    void unsubscribe(const std::string& topic)
    {
        if (m_manager.expired()) return;
        m_manager.lock()->unsubscribe(topic, m_owner);
        m_subscribed_topics.erase(topic); // Remove from cache
    }

    explicit IentityImpl(Ientity* owner) noexcept : m_owner(owner) { }

    IentityImpl(const IentityImpl& other) = delete;
    IentityImpl& operator=(const IentityImpl& other) = delete;

    ~IentityImpl() noexcept
    {
        // Unsubscribe from all cached topics
        if (!m_manager.expired()) {
            auto pmanager = m_manager.lock();
            for (const auto& topic : m_subscribed_topics) {
                try {
                    pmanager->unsubscribe(topic, m_owner);
                }
                catch (...) {
                    // Suppress exceptions in destructor to maintain noexcept guarantee
                    (void)0;
                }
            }
        }

        m_subscribed_topics.clear();
        m_manager.reset();
    }
};

Ientity::Ientity() : pImpl(std::make_shared<IentityImpl>(this))
{}

// Destructor implementation
Ientity::~Ientity() noexcept
{
    pImpl->m_destroyed = true;
}

void Ientity::destroy() noexcept
{
    pImpl->m_destroyed = true;
}

bool Ientity::is_destroyed() const noexcept
{
    return pImpl->m_destroyed;
}

size_t Ientity::get_id() const noexcept
{
    return pImpl->m_id;
}

void Ientity::set_manager(std::weak_ptr<entity_manager> manager) noexcept
{
    pImpl->set_manager(manager);
}

void Ientity::set_id(size_t id) noexcept
{
    pImpl->set_id(id);
}

// Pub/Sub methods implementation
void Ientity::subscribe(const std::string& topic)
{
    pImpl->subscribe(topic);
}

void Ientity::publish(const std::string& topic, Ientity* entity)
{
    pImpl->publish(topic, entity);
}

void Ientity::unsubscribe(const std::string& topic)
{
    pImpl->unsubscribe(topic);
}

void static_entity::set_texture(const sf::Texture& texture)
{
    m_sprite.setTexture(texture, true);
}

void static_entity::set_origin_centre() noexcept
{
    m_sprite.setOrigin({w() / 2.0f, h() / 2.0f});
}

void static_entity::set_scale(float scale_x, float scale_y) noexcept
{
    m_sprite.setScale({scale_x, scale_y});
}

void static_entity::set_position(const sf::Vector2f& pos) noexcept
{
    m_sprite.setPosition(pos);
}

void static_entity::move(const sf::Vector2f& velocity) noexcept
{
    m_sprite.move(velocity);
}

float static_entity::x() const noexcept
{
    return m_sprite.getPosition().x;
}

float static_entity::y() const noexcept
{
    return m_sprite.getPosition().y;
}

float static_entity::w() const noexcept
{
    return m_sprite.getGlobalBounds().size.x;
}

float static_entity::h() const noexcept
{
    return m_sprite.getGlobalBounds().size.y;
}

float static_entity::left() const noexcept
{
    return m_sprite.getGlobalBounds().position.x;
}

float static_entity::top() const noexcept
{
    return m_sprite.getGlobalBounds().position.y;
}

float static_entity::right() const noexcept
{
    return left() + w();
}

float static_entity::bottom() const noexcept
{
    return top() + h();
}

void moving_entity::set_velocity(const sf::Vector2f& vel) noexcept
{
    m_velocity = vel;
}

sf::Vector2f moving_entity::get_velocity() const noexcept
{
    return m_velocity;
}