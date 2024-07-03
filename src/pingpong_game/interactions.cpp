#include "interactions.hpp"
#include "wallHelper.hpp"
#include "soundplayer.hpp"

namespace interactions
{
    bool is_interacting(const entity *element1, const entity *element2) noexcept
    {
        auto box1 = element1->getGlobalbound();
        auto box2 = element2->getGlobalbound();
        return box1.intersects(box2);
    }

    std::tuple<bool, bool, bool> getDirection(ball &a_ball, entity &an_entity) noexcept
    {
        const float left_overlap = a_ball.right() - an_entity.left();
        const float right_overlap = an_entity.right() - a_ball.left();
        const float top_overlap = a_ball.bottom() - an_entity.top();
        const float bottom_overlap = an_entity.bottom() - a_ball.top();

        const bool from_left = std::abs(left_overlap) < std::abs(right_overlap);
        const bool from_top = std::abs(top_overlap) < std::abs(bottom_overlap);

        const float min_left = from_left ? std::abs(left_overlap) : std::abs(right_overlap);
        const float min_top = from_top ? std::abs(top_overlap) : std::abs(bottom_overlap);

        return std::make_tuple(min_left < min_top, from_left, from_top);
    }

    void  handle_interaction(ball &a_ball, const paddle &a_paddle)
    {
        if (is_interacting(&a_ball, &a_paddle))
        {
            SoundPlayer::getInstance()->playSound(SoundPlayer::PADDLE_BOUNCE);

            sf::Vector2f new_vel{a_ball.get_velocity()};

            new_vel.x += a_paddle.get_velocity().x * constants::paddle_damping;
            const bool new_x_signed = std::signbit(new_vel.x);
            if (abs(new_vel.x) > 0.9f * constants::ball_speed)
            {
                new_vel.x = 0.9f * constants::ball_speed * (new_x_signed ? -1.0f : 1.0f);
            }
            new_vel.y = -sqrtf(powf(constants::ball_speed, 2.0f) - powf(new_vel.x, 2.0f));
            a_ball.set_velocity(new_vel);
        }
    }

    void handle_interaction(wall &a_wall, ball &a_ball, brick &a_brick)
    {
        if (is_interacting(&a_ball, &a_brick))
        {
            
            switch (a_brick.getProperty())
            {
            case brick::BRICK:
            {
                a_brick.hit();
                auto [less, from_left, from_top] = getDirection(a_ball, a_brick);

                if (less)
                {
                    if (from_left)
                    {
                        a_ball.set_velocity({-abs(a_ball.get_velocity().x), a_ball.get_velocity().y});
                    }
                    else
                    {
                        a_ball.set_velocity({abs(a_ball.get_velocity().x), a_ball.get_velocity().y});
                    }
                    
                }
                else
                {
                    if (from_top)
                    {
                        a_ball.set_velocity({a_ball.get_velocity().x, -abs(a_ball.get_velocity().y)});
                    }
                    else
                    {
                        a_ball.set_velocity({a_ball.get_velocity().x, abs(a_ball.get_velocity().y)});
                    }
                    
                }
            }
                break;
            case brick::DIAMOND:
                a_brick.hit();
                break;
            case brick::BOMB:
            {
                wall_utils::destroyAround(a_wall, a_brick, {3, 3});
                a_ball.set_velocity({-a_ball.get_velocity().x, -a_ball.get_velocity().y});
            }
                break;
            case brick::NONE:
            {
                [[fallthrough]];
            }
            default:
                
                break;
            }
        }
    }

}

