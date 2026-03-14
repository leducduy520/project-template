#include "entitymanager.hpp"
#include "brick.hpp"
#include <algorithm>
#include <mutex>

void entity_manager::refresh()
{
    // Collect destroyed entities and their IDs
    std::vector<size_t> ids_to_recycle;

    for (auto& [type, alias_vector] : grouped_entities) {
        alias_vector.erase(remove_if(begin(alias_vector),
                                     end(alias_vector),
                                     [&ids_to_recycle](const auto& pointer) {
                                         if (pointer->is_destroyed()) {
                                             // Collect ID for recycling
                                             if (size_t id = pointer->get_id(); id > 0) {
                                                 ids_to_recycle.push_back(id);
                                             }
                                             return true;
                                         }
                                         return false;
                                     }),
                           end(alias_vector));
    }

    // Collect entities to destroy (destructors may call unsubscribe, which needs the lock)
    std::vector<std::unique_ptr<Ientity>> entities_to_destroy;
    for (auto it = all_entities.begin(); it != all_entities.end();) {
        if ((*it)->is_destroyed()) {
            // Move to temporary vector for destruction outside
            entities_to_destroy.push_back(std::move(*it));
            it = all_entities.erase(it);
        }
        else {
            ++it;
        }
    }

    // Destroy entities (destructors will unsubscribe, which needs the lock)
    entities_to_destroy.clear();

    // Recycle IDs (with lock - only ID management needs protection)
    if (!ids_to_recycle.empty()) {
        std::scoped_lock lock(m_mutex);
        for (size_t id : ids_to_recycle) {
            m_available_ids.insert(id);
        }
    }
}

// Function to destroy all entities
void entity_manager::clear() noexcept
{
    // Again, we must clean up the alias pointers first
    grouped_entities.clear();
    all_entities.clear();

    // Only lock for subscriptions and ID management
    {
        const std::scoped_lock lock(m_mutex);
        subscriptions.clear();
        // Clear the ID pool and reset counter
        m_available_ids.clear();
        m_next_entity_id = 1;
    }
}

// Get the next available ID (reuse from pool if available, otherwise generate new)
size_t entity_manager::get_next_id() noexcept
{
    const std::scoped_lock lock(m_mutex);
    if (!m_available_ids.empty()) {
        // Reuse an ID from the pool
        auto it = m_available_ids.begin();
        size_t id = *it;
        m_available_ids.erase(it);
        return id;
    }
    // Generate a new ID
    return m_next_entity_id++;
}

// Recycle an ID back to the pool
void entity_manager::recycle_id(size_t id) noexcept
{
    if (id > 0) // Don't recycle ID 0 (invalid/default)
    {
        std::scoped_lock lock(m_mutex);
        m_available_ids.insert(id);
    }
}

// Function to update all the entities
void entity_manager::update() const
{
    // Entities may call publish() which needs the lock, but we don't need to lock here
    for (auto& entity : all_entities) {
        if (!entity->is_destroyed()) {
            entity->update();
        }
    }
}

// Function to update make all the entities draw themselves
void entity_manager::draw(sf::RenderWindow& window) const
{
    for (auto& entity : all_entities) {
        if (!entity->is_destroyed()) {
            entity->draw(window);
        }
    }
}

// Pub/Sub implementation
void entity_manager::subscribe(const std::string& topic, Ientity* entity)
{
    if (entity != nullptr) {
        const std::scoped_lock lock(m_mutex);

        auto& subscribers = subscriptions[topic];
        // Check if entity is already subscribed to prevent duplicates
        if (std::find(subscribers.begin(), subscribers.end(), entity) == subscribers.end()) {
            subscribers.push_back(entity);
        }
    }
}

void entity_manager::publish(const std::string& topic, Ientity* entity)
{
    // Create a copy of subscribers list to avoid holding lock during callbacks
    std::vector<Ientity*> subscribers_copy;
    {
        const std::scoped_lock lock(m_mutex);
        auto it = subscriptions.find(topic);
        if (it != subscriptions.end()) {
            subscribers_copy = it->second; // Copy the subscribers list
        }
    }

    // Call on_message outside the lock to avoid deadlocks
    for (auto* subscriber : subscribers_copy) {
        if (subscriber != nullptr && !subscriber->is_destroyed()) {
            subscriber->on_message(topic, entity);
        }
    }
}

void entity_manager::unsubscribe(const std::string& topic, Ientity* entity)
{
    if (entity != nullptr) {
        const std::scoped_lock lock(m_mutex);
        auto it = subscriptions.find(topic);
        if (it != subscriptions.end()) {
            auto& subscribers = it->second;
            subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), entity), subscribers.end());
        }
    }
}
