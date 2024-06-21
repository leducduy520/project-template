#ifndef __WALL_HELPER__
#define __WALL_HELPER__

#include "brick.hpp"
#include "interactions.hpp"
#include <algorithm>

namespace wall_utils{
    void createWall(wall& w, const char* path);
    template <typename T>
    void interactionwith(wall& w, entity& entity)
    {
        auto elm = dynamic_cast<T*>(&entity);
        if(elm)
        {
            w.erase(std::remove_if(w.begin(), w.end(), [&](brick& br){ interactions::handle_interaction(*elm, br); return br.is_destroyed();}), w.end());
        }
        else{
            std::cerr << "The type of input entity must be derieved from entity\n";
        }
    }
}

#endif // __WALL_HELPER__