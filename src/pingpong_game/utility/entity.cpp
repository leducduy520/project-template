#include "entity.hpp"
#include "entitymanager.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

// Destructor implementation
Ientity::~Ientity() noexcept
{
    // Unsubscribe from all cached topics
    if (m_manager != nullptr) {
        for (const auto& topic : m_subscribed_topics) {
            try {
                m_manager->unsubscribe(topic, this);
            }
            catch (...) {
                // Suppress exceptions in destructor to maintain noexcept guarantee
                (void)0;
            }
        }
    }
    m_subscribed_topics.clear();
    m_manager = nullptr;
}

// Pub/Sub methods implementation
void Ientity::subscribe(const std::string& topic)
{
    if (!topic.empty() && m_manager != nullptr && m_subscribed_topics.find(topic) == m_subscribed_topics.end()) {
        m_manager->subscribe(topic, this);
        m_subscribed_topics.insert(topic); // Cache the topic
    }
}

void Ientity::publish(const std::string& topic, Ientity* entity)
{
    if (m_manager != nullptr) {
        m_manager->publish(topic, entity);
    }
}

void Ientity::unsubscribe(const std::string& topic)
{
    if (m_manager != nullptr) {
        m_manager->unsubscribe(topic, this);
        m_subscribed_topics.erase(topic); // Remove from cache
    }
}

void static_entity::set_texture(const sf::Texture& texture)
{
    m_sprite.setTexture(texture);
}

void static_entity::set_origin_centre() noexcept
{
    m_sprite.setOrigin(w() / 2.0f, h() / 2.0f);
}

void static_entity::set_scale(float scale_x, float scale_y) noexcept
{
    m_sprite.setScale(scale_x, scale_y);
}

void static_entity::set_position(const sf::Vector2f& pos) noexcept
{
    m_sprite.setPosition(pos);
}

void static_entity::move(const sf::Vector2f& velocity) noexcept
{
    m_sprite.move(velocity);
}

sf::Vector2f static_entity::get_centre() const noexcept
{
    auto bounds = m_sprite.getLocalBounds();
    return {bounds.width / 2.0f, bounds.height / 2.0f};
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
    return m_sprite.getGlobalBounds().width;
}

float static_entity::h() const noexcept
{
    return m_sprite.getGlobalBounds().height;
}

float static_entity::left() const noexcept
{
    return m_sprite.getGlobalBounds().left;
}

float static_entity::top() const noexcept
{
    return m_sprite.getGlobalBounds().top;
}

float static_entity::right() const noexcept
{
    return m_sprite.getGlobalBounds().left + m_sprite.getGlobalBounds().width;
}

float static_entity::bottom() const noexcept
{
    return m_sprite.getGlobalBounds().top + m_sprite.getGlobalBounds().height;
}

void moving_entity::set_velocity(const sf::Vector2f& vel) noexcept
{
    m_velocity = vel;
}

sf::Vector2f moving_entity::get_velocity() const noexcept
{
    return m_velocity;
}