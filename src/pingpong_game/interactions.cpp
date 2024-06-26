#include "interactions.hpp"
#include "wallHelper.hpp"

namespace interactions
{
    bool is_interacting(const entity *e1, const entity *e2) noexcept
    {
        auto box1 = e1->getGlobalbound();
        auto box2 = e2->getGlobalbound();
        return box1.intersects(box2);
    }

    std::tuple<bool, bool, bool> getDirection(ball &b, entity &e) noexcept
    {
        const float left_overlap = b.right() - e.left();
        const float right_overlap = e.right() - b.left();
        const float top_overlap = b.bottom() - e.top();
        const float bottom_overlap = e.bottom() - b.top();

        const bool from_left = std::abs(left_overlap) < std::abs(right_overlap);
        const bool from_top = std::abs(top_overlap) < std::abs(bottom_overlap);

        const float min_left = from_left ? std::abs(left_overlap) : std::abs(right_overlap);
        const float min_top = from_top ? std::abs(top_overlap) : std::abs(bottom_overlap);

        return std::make_tuple(min_left < min_top, from_left, from_top);
    }

    void  handle_interaction(ball &b, const paddle &p)
    {
        if (is_interacting(&b, &p))
        {


            sf::Vector2f new_vel{b.get_velocity()};
            const bool x_signed = std::signbit(new_vel.x);
            const bool y_signed = std::signbit(new_vel.y);

            new_vel.x += p.get_velocity().x * constants::paddle_damping;
            const bool new_x_signed = std::signbit(new_vel.x);
            if (abs(new_vel.x) > 0.9f * constants::ball_speed)
                new_vel.x = 0.9f * constants::ball_speed * new_x_signed ? -1.0f : 1.0f;
            new_vel.y = -sqrtf(powf(constants::ball_speed, 2.0f) - powf(new_vel.x, 2.0f));
            b.set_velocity(std::move(new_vel));
        }
    }

    void handle_interaction(wall &w, ball &b, brick &br)
    {
        if (is_interacting(&b, &br))
        {
            
            switch (br.getProperty())
            {
            case brick::BRICK:
            {
                br.hit();
                
                auto [less, from_left, from_top] = getDirection(b, br);

                if (less)
                {
                    if (from_left)
                    {
                        b.set_velocity({-abs(b.get_velocity().x), b.get_velocity().y});
                    }
                    else
                    {
                        b.set_velocity({abs(b.get_velocity().x), b.get_velocity().y});
                    }
                    
                }
                else
                {
                    if (from_top)
                    {
                        b.set_velocity({b.get_velocity().x, -abs(b.get_velocity().y)});
                    }
                    else
                    {
                        b.set_velocity({b.get_velocity().x, abs(b.get_velocity().y)});
                    }
                    
                }
            }
                break;
            case brick::DIAMOND:
                br.hit();
                break;
            case brick::BOMB:
            {
                wall_utils::destroyAround(w, br, {3, 3});
                b.set_velocity({-b.get_velocity().x, -b.get_velocity().y});
            }
                break;
            default:
                
                break;
            }
        }
    }

}

