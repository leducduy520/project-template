#include "interactions.hpp"
#include "helper.hpp"
#include "soundplayer.hpp"
#include <algorithm>
#include <future>
#include <mutex>
#include <random>
#include <thread>

namespace interactions
{
    bool is_interacting(const entity* element1, const entity* element2) noexcept
    {
        auto box1 = element1->getGlobalbound();
        auto box2 = element2->getGlobalbound();
        return box1.intersects(box2);
    }

    std::tuple<bool, bool, bool> getDirection(ball& a_ball, entity& an_entity) noexcept
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

    void handle_interaction(ball& a_ball, const paddle& a_paddle)
    {
        if (is_interacting(&a_ball, &a_paddle))
        {
            SoundPlayer::getInstance()->playSound(SoundPlayer::PADDLE_BOUNCE);

            sf::Vector2f new_vel{a_ball.get_velocity()};

            new_vel.x += a_paddle.get_velocity().x * constants::paddle_damping;
            const bool new_x_signed = std::signbit(new_vel.x);
            if (std::abs(new_vel.x) > 0.9F * constants::ball_speed)
            {
                new_vel.x = 0.9f * constants::ball_speed * (new_x_signed ? -1.0f : 1.0f);
            }
            new_vel.y = -sqrtf(powf(constants::ball_speed, 2.0f) - powf(new_vel.x, 2.0f));
            a_ball.set_velocity(new_vel);
        }
    }

    void handle_interaction(wall& a_wall, ball& a_ball)
    {
        for (auto it = a_wall.begin(); it != a_wall.end(); ++it)
        {
            auto* a_brick = it->second.get();
            if (is_interacting(&a_ball, a_brick))
            {
                switch (a_brick->getProperty())
                {
                case brick::BRICK:
                {
                    interation_ball_n_brick(a_brick, a_ball);
                }
                break;
                case brick::DIAMOND:
                {
                    interation_ball_n_diamond(a_brick, a_ball);
                }
                break;
                case brick::BOMB:
                {
                    interation_ball_n_bomb(a_brick, a_ball);
                }
                break;
                case brick::SCALEUP:
                {
                    interation_ball_n_scaleup(a_brick, a_ball);
                }
                break;
                case brick::CLONE:
                {
                    interation_ball_n_clone(a_brick, a_ball);
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

        a_wall.refresh();
    }

    void interation_ball_n_clone(brick* a_brick, ball& a_ball)
    {
        a_brick->hit(a_ball.get_strength());
        a_ball.clone();
    }

    void interation_ball_n_scaleup(brick* a_brick, ball& a_ball)
    {
        a_brick->hit(a_ball.get_strength());
        a_ball.scale(2);
    }

    void interation_ball_n_bomb(brick* a_brick, ball& a_ball)
    {
        const std::unique_lock ulock(utilities::random::rd_mutex);
        utilities::wallhelper::destroyAround(*a_brick, {3, 3});
        auto do_random = []() -> bool { return utilities::random::bernoulli_dist(utilities::random::gen); };
        do_random() ? a_ball.move_right() : a_ball.move_left();
        do_random() ? a_ball.move_up() : a_ball.move_down();
    }

    void interation_ball_n_diamond(brick* a_brick, ball& a_ball)
    {
        a_brick->hit(a_ball.get_strength());
    }

    void interation_ball_n_brick(brick* a_brick, ball& a_ball)
    {
        a_brick->hit(a_ball.get_strength());
        auto [more_at_side, from_left, from_top] = getDirection(a_ball, *a_brick);

        if (more_at_side)
        {
            from_left ? a_ball.move_left() : a_ball.move_right();
        }
        else
        {
            from_top ? a_ball.move_up() : a_ball.move_down();
        }
    }

} // namespace interactions
