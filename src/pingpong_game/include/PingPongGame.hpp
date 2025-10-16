#ifndef GAME_H
#define GAME_H

#include "IGame.h"
#include "background.hpp"
#include "ball.hpp"
#include "brick.hpp"
#include "constants.hpp"
#include "countingtext.hpp"
#include "entitymanager.hpp"
#include "nlohmann/json.hpp"
#include "paddle.hpp"
#include <vector>

class PingPongGame : public IGame
{

    sf::RenderWindow game_window{{constants::window_width, constants::window_height},
                                 "Simple Breakout Game Version 9",
                                 sf::Style::None};
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
    sf::Text m_textPoint;

    CountingText m_countingText;

    bool savedData;
    int64_t m_GameSessionID;
    std::string m_username;
    uint16_t m_point;
    uint8_t m_live;

    static std::string toJsonString(const uint8_t* data, size_t length);
    static nlohmann::json toJson(const uint8_t* data, size_t length);

public:
    PingPongGame();

    //! Database implementation
    static void updateGameRecord();
    int64_t updateGameSessionID();
    void databaseResultUpdate(const game_state& state);
    void databaseRetryUpdate();
    void removeCurrentData();
    void updateGameNewHistory();
    void updateGameSessionEndTime();
    void updateGameSessionStartTime();

    //! Game implementation
    void init(std::string& resourcePath) override;
    void reset();
    void clear();
    void run() override;
    void listening();
    void stateHandler();
    void update();
    void render();

    //! Support function
    void try_createwall();
    void try_login();
    void initialize_text();
    void initialize_sound_settings();
    void check_finish_by_ball();
    void check_finish_by_wall();
    void handleKeyPressed_P(bool& pause_key_active);
    void handleKeyPressed_R(bool& reset_key_active);
};

#endif // GAME_H
