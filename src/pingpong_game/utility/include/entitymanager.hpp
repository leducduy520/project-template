#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include "entity.hpp"
#include <map>
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <typeinfo>
#include <set>
#include <mutex>

using entity_vector = std::vector<std::unique_ptr<Ientity>>;
using entity_alias_vector = std::vector<Ientity*>;

class entity_manager : public std::enable_shared_from_this<entity_manager>
{
    // A vector to store all the entities in the game (all brick objects, background, ball, paddle)
    entity_vector all_entities;

    // A map to store the entities grouped by type
    // We can iterate over this to get a vector of all brick objects, then a vector of all ball objects, etc
    // The vector will contain non-owning pointers to the objects
    // Do not delete these pointers!
    // Do not use them after the objects they point to have been destroyed!
    std::map<size_t, entity_alias_vector> grouped_entities;

    // Pub/Sub system: map of topic names to subscriber entity pointers
    std::map<std::string, std::vector<Ientity*>, std::less<>> subscriptions;

    // ID management: pool of recycled IDs and next available ID
    std::set<size_t> m_available_ids;
    size_t m_next_entity_id{1};

    // Mutex for thread safety
    mutable std::mutex m_mutex;

    // Get the next available ID (reuse from pool if available, otherwise generate new)
    size_t get_next_id() noexcept;

    // Recycle an ID back to the pool
    void recycle_id(size_t id) noexcept;

public:
    // Function to create an entity object of type T using args as the constructor arguments
    // We use a variadic template to pass any number of arguments
    // We use perfect forwarding to avoid any unnecessary copying of the arguments
    template <typename T, typename... Args>
    T& create(Args&&... args)
    {
        // Check that the type parameter is derived from the entity base class
        static_assert(std::is_base_of_v<Ientity, T>, R"("T" type parameter in create() must be derived from "entity")");

        // Create a unique_ptr to the entity
        // Forward the arguments to the entity's constructor
        auto ptr{std::make_unique<T>(std::forward<Args>(args)...)};

        // Make an alias pointer to the allocated memory
        // This will be stored in the entity_type_vector
        auto ptr_alias = ptr.get();

        // Get the hash code for the entity object's type
        auto hash = typeid(T).hash_code();

        // Get ID (this locks for ID management)
        size_t new_id = get_next_id();

        // Insert the alias pointer into the map (no lock needed)
        grouped_entities[hash].emplace_back(ptr_alias);

        // Insert the object's pointer into the entities vector (no lock needed)
        all_entities.emplace_back(std::move(ptr));

        // Set the manager reference and ID in the entity
        ptr_alias->set_manager(weak_from_this());
        ptr_alias->set_id(new_id);

        // Return the new object
        return *ptr_alias;
    }

    // Function to scan all entities and clean up the destroyed ones
    void refresh();

    // Function to destroy all entities
    void clear() noexcept;

    // Function to retrieve all the objects of a given type
    template <typename T>
    auto& get_all()
    {
        return grouped_entities[typeid(T).hash_code()];
    }

    // Apply a function to all entities of a given type
    template <typename T, typename... Func>
    void apply_all(Func&&... func)
    {
        auto& entity_group = get_all<T>();

        for (auto ptr : entity_group) {
            auto* entity = dynamic_cast<T*>(ptr);

            // Apply each function in the parameter pack to the entity using an initializer list
            //(void)std::initializer_list<int>{ (func(*entity), 0)... }; c++11 or higher

            // Apply each function in the parameter pack to the entity using a fold expression
            (..., func(*entity)); //c++17 or higher
        }
    }

    // Function to update all the entities
    void update() const;

    // Function to make all the entities draw themselves
    void draw(sf::RenderWindow& window) const;

    // Pub/Sub methods
    // Subscribe an entity to a topic
    void subscribe(const std::string& topic, Ientity* entity);

    // Publish an event to a topic
    void publish(const std::string& topic, Ientity* entity);

    // Unsubscribe an entity from a topic
    void unsubscribe(const std::string& topic, Ientity* entity);
};

#endif // __ENTITY_MANAGER_H__
