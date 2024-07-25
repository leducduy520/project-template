#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

// Code for a "breakout" game
// Based on a talk by Vittorio Romeo and code by "FamTrimli"
// Uses the SFML graphics library

struct constants
{
    static constexpr int window_width{520};
    static constexpr int window_height{450};
    static constexpr float ball_speed{8.0f};
    static constexpr int paddlle_width{120};
    static constexpr int paddlle_height{40};
    static constexpr float paddle_speed{8.0f};
    static constexpr float paddle_accel{1.50f};
    static constexpr float paddle_decel{1.50f};
    static constexpr float paddle_damping{0.20f};
    static constexpr int brick_width{32};
    static constexpr int brick_height{32};
    static constexpr int brick_verical_lanes{13};
    static constexpr int brick_horizontal_lanes{6};
    static constexpr int cap_brick_hit{5};
    static constexpr int cap_diamond_hit{1};
    static constexpr int cap_bomb_hit{1};
    static constexpr int fmtnow{ std::size("0000-00-00 00:00:00 GMT") };
    static std::string resoucesPath;
};

#endif // CONSTANTS_H
