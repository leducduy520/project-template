#include "interactions.hpp"

namespace interactions
{
    bool  is_interacting(const entity *e1, const entity *e2)
    {
        auto box1 = e1->get_bounding_box();
        auto box2 = e2->get_bounding_box();
        return box1.intersects(box2);
    }

    const std::pair<float, float> get_raito(const ball &b, const entity &p)
    {
        const float length{static_cast<float>(sqrt(2)*constants::ball_speed)}; //8,48528137423857
        const double angle{abs((b.left() + b.width() / 2) - (p.left() + p.width() / 2)) / (p.width() / 2.0f) * 60};
        const double c = cos(angle * 3.14 / 180.0f);
        const double s = sqrt(1.0f - pow(c , 2));
        const float x_ratio = s * length;
        const float y_ratio = c * length;
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

            const auto [x_ratio, y_ratio] = get_raito(b, br);
            if (from_left)
            {
                b.move_left(x_ratio);
            }
            else
            {
                b.move_right(x_ratio);
            }
            if (from_top)
            {
                b.move_up(y_ratio);
            }
            else
            {
                b.move_down(y_ratio);
            }
        }
    }

}

