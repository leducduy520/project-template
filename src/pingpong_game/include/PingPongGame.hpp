#ifndef GAME_H
#define GAME_H

#include "IGame.h"
#include "background.hpp"
#include "ball.hpp"
#include "brick.hpp"
#include "constants.hpp"
#include "entityManager.hpp"
#include "paddle.hpp"

class PingPongGame : public IGame
{

    sf::RenderWindow game_window{{constants::window_width, constants::window_height}, "Simple Breakout Game Version 9"};
    entity_manager m_entity_manager;
    enum class game_state
    {
        paused,
        running,
        game_over,
        player_wins
    };
    game_state m_state{game_state::running};

    sf::Font m_font;
    sf::Text m_textState;
    sf::Text m_textLive;

    int m_live;

    void eventHandler();
    void stateHandler();
    void update();
    void render();
    void try_createwall();

    static void centeredText(sf::Text &text);

public:
    explicit PingPongGame(std::string resourcePath);
    PingPongGame();

    void init(std::string &resourcePath) override;
    // Reinitialize the game
    void reset();

    void clear();

    // Game loop
    void run() override;
};

#endif // GAME_H
