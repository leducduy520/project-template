#ifndef GAME_H
#define GAME_H

#include "IGame.h"
#include "background.hpp"
#include "ball.hpp"
#include "brick.hpp"
#include "constants.hpp"
#include "entityManager.hpp"
#include "paddle.hpp"
#include "bson/bson.h"
#include "nlohmann/json.hpp"

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

    bool savedData;
    int64_t m_GameSessionID;
    std::string m_username;
    uint16_t m_point;
    uint8_t m_live;

    static std::string toJsonString(const uint8_t* data, size_t length);
    static nlohmann::json toJson(const uint8_t* data, size_t length);
    int64_t updateGameSessionID();

    void databaseResultUpdate(const bool& isWin);
    void databaseRetryUpdate();
    void listening();
    void removeCurrentData();
    void render();
    void stateHandler();
    void try_createwall();
    void try_database();
    void update();
    void updateGameNewHistory();
    static void updateGameRecord();
    void updateGameSessionEndTime();
    void updateGameSessionStartTime();

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
