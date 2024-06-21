#ifndef __INTERACTIONS__

#define __INTERACTIONS__

#include "ball.hpp"
#include "brick.hpp"
#include "paddle.hpp"
#include <cmath>
#include <map>


bool  is_interacting(const entity *e1, const entity *e2);

const std::pair<float, float> get_raito(const ball &b, const entity &p);

void  handle_interaction(ball &b, const paddle &p);

void  handle_interaction(ball &b, brick &br);

#endif // __INTERACTIONS__
