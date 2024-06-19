#ifndef __INTERACTIONS__

#define __INTERACTIONS__

#include "ball.hpp"
#include "brick.hpp"
#include "paddle.hpp"
#include <cmath>
#include <map>


bool  is_interacting(const entity *e1, const entity *e2)
{
    auto box1 = e1->get_bounding_box();
    auto box2 = e2->get_bounding_box();
    return box1.intersects(box2);
}

const std::pair<float, float> get_raito(const ball &b, const paddle &p)
{
    const float length{sqrt(powf(constants::ball_speed, 2.0f) * 2 )}; //8,48528137423857
    const double angle{abs(b.x() - p.x()) / p.get_bounding_box().width * 75};
    const double c = cos(angle * 3.14 / 180);
    const double s = sqrt(1 - powf(c , 2.0f));
    const float x_ratio = s * length / constants::ball_speed;
    const float y_ratio = c * length / constants::ball_speed;
    return std::make_pair(x_ratio, y_ratio);
}

void  handle_interaction(ball &b, const paddle &p)
{
    if (is_interacting(&b, &p))
    {
        const auto [x_ratio, y_ratio] = get_raito(b, p);
        b.move_up(y_ratio);
        if (b.x() < p.x())
        {
            b.move_left(x_ratio);
        }
        else
        {
            b.move_right(x_ratio);
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