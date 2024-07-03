#ifndef __INTERACTIONS__

#define __INTERACTIONS__

#include "ball.hpp"
#include "brick.hpp"
#include "paddle.hpp"
#include <cmath>
#include <map>

namespace interactions
{
    bool is_interacting(const entity *element1, const entity *element12) noexcept;

    std::tuple<bool, bool, bool> getDirection(ball &a_ball, entity &an_entity) noexcept;

    void handle_interaction(ball &a_ball, const paddle &a_paddle);

    void handle_interaction(wall &a_wall, ball &a_ball, brick &a_brick);
}


#endif // __INTERACTIONS__
