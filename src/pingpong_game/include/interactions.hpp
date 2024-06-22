#ifndef __INTERACTIONS__

#define __INTERACTIONS__

#include "ball.hpp"
#include "brick.hpp"
#include "paddle.hpp"
#include <cmath>
#include <map>

namespace interactions
{
    bool  is_interacting(const entity *e1, const entity *e2) noexcept;

    std::tuple<bool, bool, bool> getDirection(ball &b, entity &e) noexcept;

    void  handle_interaction(ball &b, const paddle &p);

    void handle_interaction(wall& w, ball &b, brick &br);
}


#endif // __INTERACTIONS__
