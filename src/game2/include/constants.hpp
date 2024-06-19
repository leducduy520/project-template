#ifndef CONSTANTS_H
#define CONSTANTS_H



// Code for a "breakout" game
// Based on a talk by Vittorio Romeo and code by "FamTrimli"
// Uses the SFML graphics library

struct constants
{
    static constexpr int window_width{520};
    static constexpr int window_height{450};
    static constexpr float ball_speed{6.0f};
    static constexpr int paddlle_width{120};
    static constexpr int paddlle_height{40};
    static constexpr float paddle_speed{4.0f};
    static constexpr int brick_width{32};
    static constexpr int brick_height{20};
    static constexpr int brick_verical_lanes{13};
    static constexpr int brick_horizontal_lanes{6};
    static constexpr int brick_padding{(window_width - brick_verical_lanes * brick_width) / 2};
};

#endif // CONSTANTS_H
