#ifndef __INTERACTIONS__

#define __INTERACTIONS__

#include "ball.hpp"
#include "brick.hpp"
#include "paddle.hpp"
#include "entitymanager.hpp"

namespace interactions
{
    bool are_interacting(const static_entity* element1, const static_entity* element12) noexcept;

    class BallvsPaddle
    {
        ball& m_ball;
        const paddle& m_paddle;

    public:
        BallvsPaddle(ball& a_ball, const paddle& a_paddle);
        void operator()();
    };

    class BallVsWall
    {
        wall& m_wall;
        ball& m_ball;
        entity_manager& m_entity_manager;
        bool m_should_create_counting_text{false};

    public:
        BallVsWall(wall& a_wall, ball& a_ball, entity_manager& entity_mgr);
        void operator()();

    private:
        std::tuple<bool, bool, bool> cal_ball_direction(const ball& a_ball,
                                                        const static_entity& an_entity) const noexcept;

        void ball_interact_clone_brick(brick& a_brick);
        void ball_interact_scaleup_brick(brick& a_brick);
        void ball_interact_bomb_brick(brick& a_brick);
        void ball_interact_diamond_brick(brick& a_brick);
        void ball_interact_normal_brick(brick& a_brick);
    };


} // namespace interactions


#endif // __INTERACTIONS__
