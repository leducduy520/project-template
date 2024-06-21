#include "entityManager.hpp"
#include "brick.hpp"

void entity_manager::refresh() {
	for (auto& [type, alias_vector] : grouped_entities) {

		alias_vector.erase(remove_if(begin(alias_vector), end(alias_vector),
				[](const auto& p) { return p->is_destroyed(); }
				),
		end(alias_vector));
	}

	// Now we can safely destroy the objects, now that there are no aliases to them
	all_entities.erase(remove_if(begin(all_entities), end(all_entities),
			[](const auto& p) { return p->is_destroyed(); }
			),
	end(all_entities));
}

// Function to destroy all entities
void entity_manager::clear() {
	// Again, we must clean up the alias pointers first
	grouped_entities.clear();
	all_entities.clear();
}

// Function to update all the entities
void entity_manager::update() {
	for (auto& e : all_entities)
		e->update();
}

// Function to update make all the entities draw themselves
void entity_manager::draw(sf::RenderWindow& window) {
	for (auto& e : all_entities)
		e->draw(window);
}
