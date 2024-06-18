#ifndef __INTERACTIONS__

#define __INTERACTIONS__

#include "ball.hpp"
#include "brick.hpp"
#include "paddle.hpp"

#ifdef _WIN32
  #ifdef GAME_1_EXPORTS
    #define GAME_1_API __declspec(dllexport)
  #else
    #define GAME_1_API __declspec(dllimport)
  #endif
#else
  #define GAME_1_API
#endif

bool  is_interacting(const entity *e1, const entity *e2)
{
    auto box1 = e1->get_bounding_box();
    auto box2 = e2->get_bounding_box();
    return box1.intersects(box2);
}

void  handle_interaction(ball &b, const paddle &p)
{
    if (is_interacting(&b, &p))
    {
        b.move_up();
        if (b.x() < p.x())
        {
            b.move_left();
        }
        else
        {
            b.move_right();
        }
    }
}

void  handle_interaction(ball &b, brick &br)
{
    if (is_interacting(&b, &br))
    {
        br.destroy();

        float left_overlap = b.right() - br.left();
        float right_overlap = br.right() - b.left();
        float top_overlap = b.bottom() - br.top();
        float bottom_overlap = br.bottom() - b.top();

        bool from_left = std::abs(left_overlap) < std::abs(right_overlap);
        bool from_top = std::abs(top_overlap) < std::abs(bottom_overlap);

        float min_x_overlap = from_left ? left_overlap : right_overlap;
        float min_y_overlap = from_top ? top_overlap : bottom_overlap;
        if (min_x_overlap < min_y_overlap)
        {
            if (from_left)
            {
                b.move_left();
            }
            else
            {
                b.move_right();
            }
        }
        else
        {
            if (from_top)
            {
                b.move_up();
            }
            else
            {
                b.move_down();
            }
        }
    }
}

#endif // __INTERACTIONS__