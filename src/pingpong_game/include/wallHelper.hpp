#ifndef __WALL_HELPER__
#define __WALL_HELPER__

#include "brick.hpp"
#include "interactions.hpp"
#include <algorithm>

namespace wall_utils{
    void createWall(wall& w, const char* path);
    void destroyAround(wall& w, brick& br, sf::Vector2i range);
    void checkAlive(wall &w);
    template <typename T>
    void interactionwith(wall& w, entity& entity)
    {
        auto elm = dynamic_cast<T*>(&entity);
        if(elm)
        {
            for (auto it = w.begin(); it != w.end(); ++it) {
                interactions::handle_interaction(w, *elm, *(it->second.get()));
            }
        }
        else{
            std::cerr << "The type of input entity must be derieved from entity\n";
        }
    }
}

#endif // __WALL_HELPER__