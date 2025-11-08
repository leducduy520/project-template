#include "interactions.hpp"
#include "helper.hpp"
#include "soundplayer.hpp"
#include <execution>

namespace interactions
{
    bool are_interacting(const entity* element1, const entity* element2) noexcept
    {
        auto box1 = element1->get_global_bound();
        auto box2 = element2->get_global_bound();
        return box1.intersects(box2);
    }

    BallvsPaddle::BallvsPaddle(ball& a_ball, const paddle& a_paddle) : m_ball(a_ball), m_paddle(a_paddle)
    {}

    void BallvsPaddle::operator()()
    {
        if (are_interacting(&m_ball, &m_paddle))
        {
            SoundPlayer::playSoundEffect(SoundPlayer::SoundEffect_t::PADDLE_BOUNCE);

            sf::Vector2f new_vel{m_ball.get_velocity()};

            new_vel.x += m_paddle.get_velocity().x * constants::paddle_damping;
            const bool new_x_signed = std::signbit(new_vel.x);
            if (std::abs(new_vel.x) > 0.9F * constants::ball_speed)
            {
                new_vel.x = 0.9f * constants::ball_speed * (new_x_signed ? -1.0f : 1.0f);
            }
            new_vel.y = -sqrtf(powf(constants::ball_speed, 2.0f) - powf(new_vel.x, 2.0f));
            m_ball.set_velocity(new_vel);
        }
    }

    BallVsWall::BallVsWall(wall& a_wall, ball& a_ball) : m_wall(a_wall), m_ball(a_ball)
    {}

    void BallVsWall::operator()()
    {
        std::mutex mtx;
        auto& wall_data = m_wall.data();
        std::for_each(std::execution::par_unseq, wall_data.begin(), wall_data.end(), [this, &mtx](auto& it) {
            auto& a_brick = *it.second.get();
            if (are_interacting(&m_ball, &a_brick))
            {
                std::scoped_lock lock(mtx);
                switch (a_brick.getProperty())
                {
                case brick::BRICK:
                {
                    ball_interact_normal_brick(a_brick);
                }
                break;
                case brick::DIAMOND:
                {
                    ball_interact_diamond_brick(a_brick);
                }
                break;
                case brick::BOMB:
                {
                    ball_interact_bomb_brick(a_brick);
                }
                break;
                case brick::SCALEUP:
                {
                    ball_interact_scaleup_brick(a_brick);
                }
                break;
                case brick::CLONE:
                {
                    ball_interact_clone_brick(a_brick);
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
        });

        m_wall.refresh();
    }

    std::tuple<bool, bool, bool> BallVsWall::cal_ball_direction(ball& a_ball, entity& an_entity) noexcept
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

    void BallVsWall::ball_interact_clone_brick(brick& a_brick)
    {
        a_brick.hit(m_ball.get_strength());
        m_ball.clone();
    }

    void BallVsWall::ball_interact_scaleup_brick(brick& a_brick)
    {
        a_brick.hit(m_ball.get_strength());
        m_ball.scale(2);
    }

    void BallVsWall::ball_interact_bomb_brick(brick& a_brick)
    {
        utilities::wallhelper::destroy_around(a_brick, {3, 3});
        using namespace utilities::random;
        const std::function<bool(std::bernoulli_distribution&, std::mt19937&)> do_random =
            [](std::bernoulli_distribution& dist, std::mt19937& gen) { return dist(gen); };
        generator::getRandomBoolean(do_random) ? m_ball.move_right() : m_ball.move_left();
        generator::getRandomBoolean(do_random) ? m_ball.move_up() : m_ball.move_down();
    }

    void BallVsWall::ball_interact_diamond_brick(brick& a_brick)
    {
        a_brick.hit(m_ball.get_strength());
    }

    void BallVsWall::ball_interact_normal_brick(brick& a_brick)
    {
        a_brick.hit(m_ball.get_strength());
        auto [more_at_side, from_left, from_top] = cal_ball_direction(m_ball, a_brick);

        if (more_at_side)
        {
            from_left ? m_ball.move_left() : m_ball.move_right();
        }
        else
        {
            from_top ? m_ball.move_up() : m_ball.move_down();
        }
    }

} // namespace interactions
