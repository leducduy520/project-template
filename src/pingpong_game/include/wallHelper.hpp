#ifndef __WALL_HELPER__
#define __WALL_HELPER__

#include "brick.hpp"
#include "interactions.hpp"
#include <algorithm>

namespace wall_utils{
    void createWall(wall& w, const char* path);
    void destroyAround(wall& w, brick& br, sf::Vector2i range);
    template <typename T>
    void interactionwith(wall& w, entity& entity)
    {
        auto elm = dynamic_cast<T*>(&entity);
        if(elm)
        {
            for (auto it = w.begin(); it != w.end(); ++it) {
                interactions::handle_interaction(w, *elm, *(it->second.get()));
            }
            for (auto it = w.rbegin(); it != w.rend();)
            {
                if (it->second->is_destroyed())
                {
                    w.erase(it->first);
                }
                else
                {
                    ++it;
                }
            }
        }
        else{
            std::cerr << "The type of input entity must be derieved from entity\n";
        }
    }
}

#endif // __WALL_HELPER__