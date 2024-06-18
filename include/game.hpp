#ifndef __GAME_1__
#define __GAME_1__

#include "background.hpp"
#include "ball.hpp"
#include "brick.hpp"
#include "paddle.hpp"
#include <list>

class game1
{
    paddle _paddle{constants::window_width / 2.0f, constants::window_height};
    ball _ball{constants::window_width / 2.0f, constants::window_height / 2.0f};
    background _background{constants::window_width, constants::window_height};
    std::list<brick> _bricks;

public:
    game1();

    enum GameState{
        PLAYING,
        PAUSED,
        GAME_OVER
    };

    void run();

private:
    void getState(GameState state) noexcept;
    GameState getState() const noexcept;
    GameState _gameState{PAUSED};
};
#endif // __GAME_1__