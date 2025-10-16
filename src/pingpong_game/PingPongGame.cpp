#include "PingPongGame.hpp"
#include "DBClientGame.hpp"
#include "LoginGame.hpp"
#include "ThreadPoolGame.hpp"
#include "helper.hpp"
#include "interactions.hpp"
#include "soundplayer.hpp"

std::string constants::resouces_path;
using namespace std;
using namespace std::literals;
using namespace utilities::texthelper;
using json = nlohmann::json;

extern void CountingTextUpdate(CountingText* text);

void PingPongGame::updateGameSessionStartTime()
{
    std::array<char, constants::fmt_now> buffer{};
    std::tm tmbuff{0};

#if defined(_WIN32) || defined(_WIN64)
    gmtime_s(&tmbuff, &m_GameSessionID);
#else
    gmtime_r(&m_GameSessionID, &tmbuff);
#endif

    strftime(buffer.data(), constants::fmt_now, "%F %T GMT", &tmbuff);

    DBINSTANCE->UpdateDocument(make_document(kvp("name", m_username), kvp("history.id", m_GameSessionID)),
                               make_document(kvp("$set", make_document(kvp("history.$.start_time", buffer.data())))));
}

std::string PingPongGame::toJsonString(const uint8_t* data, size_t length)
{
    bson_t bson;
    bson_init_static(&bson, data, length);

    size_t size{};
    auto* result = bson_array_as_json(&bson, &size);

    if (result != nullptr)
    {
        return {};
    }

    const auto deleter = [](char* result) { bson_free(result); };
    const std::unique_ptr<char[], decltype(deleter)> cleanup(result, deleter);

    return {result, size};
}

nlohmann::json PingPongGame::toJson(const uint8_t* data, size_t length)
{
    return json{toJsonString(data, length)};
}

void PingPongGame::updateGameSessionEndTime()
{
    if (!savedData)
    {
        auto oldGameSessionID = updateGameSessionID();

        std::array<char, constants::fmt_now> buffer{};
        std::tm tmbuff{0};

#if defined(_WIN32) || defined(_WIN64)
        gmtime_s(&tmbuff, &m_GameSessionID);
#else
        gmtime_r(&m_GameSessionID, &tmbuff);
#endif

        strftime(buffer.data(), constants::fmt_now, "%F %T GMT", &tmbuff);

        auto duration = minus<decltype(m_GameSessionID)>{}(m_GameSessionID, oldGameSessionID);

        // DBINSTANCE->UpdateDocument(make_document(kvp("name", m_username), kvp("history.id", oldGameSessionID)),
        //                            make_document(kvp("$set",
        //                                              make_document(kvp("history.$.end_time", buffer.data()),
        //                                                            kvp("history.$.duration", duration)))));
        // updateGameRecord();
        savedData = true;
    }
}

void PingPongGame::updateGameRecord()
{
    mongocxx::v_noabi::pipeline pipeline;
    mongocxx::v_noabi::options::aggregate opts;

    opts.allow_disk_use(true);
    opts.max_time(std::chrono::milliseconds{60000});

    pipeline
        .add_fields(make_document(
            kvp("record",
                make_document(kvp("$slice",
                                  make_array(make_document(kvp("$sortArray",
                                                               make_document(kvp("input", "$history"),
                                                                             kvp("sortBy",
                                                                                 make_document(kvp("score", -1),
                                                                                               kvp("duration", 1),
                                                                                               kvp("live", -1),
                                                                                               kvp("id", 1)))))),
                                             3))))))
        .merge(make_document(kvp("into", make_document(kvp("db", "duyld"), kvp("coll", "pingpong_game")))));
    DBINSTANCE->RunPipeLine(pipeline, opts);
}

void PingPongGame::updateGameNewHistory()
{
    DBINSTANCE->UpdateDocument(make_document(kvp("name", m_username)),
                               make_document(kvp("$push",
                                                 make_document(kvp("history",
                                                                   make_document(kvp("id", m_GameSessionID),
                                                                                 kvp("end_time", ""),
                                                                                 kvp("result", ""),
                                                                                 kvp("score", 0),
                                                                                 kvp("live", 3)))))

                                                 ));
    updateGameSessionStartTime();
    savedData = false;
}

int64_t PingPongGame::updateGameSessionID()
{
    return std::exchange(
        m_GameSessionID,
        std::chrono::duration_cast<std::chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count());
}

void PingPongGame::databaseRetryUpdate()
{
    if (m_state == game_state::running || m_state == game_state::paused)
    {
        databaseResultUpdate(game_state::game_over);
    }
    updateGameNewHistory();
}

void PingPongGame::databaseResultUpdate(const game_state& state)
{
    if (game_state::player_wins == state)
    {
        DBINSTANCE->UpdateDocument(
            make_document(kvp("name", m_username), kvp("history.id", m_GameSessionID)),
            make_document(kvp("$set",
                              make_document(kvp("history.$.result", "win"),
                                            kvp("history.$.live", m_live),
                                            kvp("history.$.score",
                                                static_cast<int32_t>(m_point) + static_cast<int32_t>(20 * m_live))))));
    }
    else if (game_state::game_over == state)
    {
        DBINSTANCE->UpdateDocument(
            make_document(kvp("name", m_username), kvp("history.id", m_GameSessionID)),
            make_document(kvp("$set",
                              make_document(kvp("history.$.result", "lose"),
                                            kvp("history.$.live", m_live),
                                            kvp("history.$.score", static_cast<int32_t>(m_point))))));
    }
    updateGameSessionEndTime();
}

void PingPongGame::removeCurrentData()
{
    DBINSTANCE->UpdateDocument(make_document(kvp("name", m_username)),
                               make_document(kvp("$pop", make_document(kvp("history", 1)))));
}

void PingPongGame::listening()
{
    static bool pause_key_active = false;
    static bool reset_key_active = false;
    sf::Event event{};

    while (game_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            // (m_state == game_state::running || m_state == game_state::paused) ? removeCurrentData()
            //                                                                   : updateGameSessionEndTime();
            game_window.close();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
    {
        // (m_state == game_state::running || m_state == game_state::paused) ? removeCurrentData()
        //                                                                   : updateGameSessionEndTime();
        game_window.close();
    }

    handleKeyPressed_P(pause_key_active);

    handleKeyPressed_R(reset_key_active);
}

void PingPongGame::handleKeyPressed_R(bool& reset_key_active)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
    {
        if (!reset_key_active)
        {
            // databaseRetryUpdate();
            reset();
        }
        reset_key_active = true;
    }
    else
    {
        reset_key_active = false;
    }
}

void PingPongGame::handleKeyPressed_P(bool& pause_key_active)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P))
    {
        // If it was not pressed on the last iteration, toggle the status
        if (!pause_key_active)
        {
            if (m_state == game_state::paused)
            {
                m_state = game_state::running;
                m_countingText.stop_pause();
                m_entity_manager.apply_all<ball>([this](ball& a_ball) { a_ball.set_pause(false); });
            }
            else if (m_state == game_state::running)
            {
                m_state = game_state::paused;
                m_countingText.pause();
                m_entity_manager.apply_all<ball>([this](ball& a_ball) { a_ball.set_pause(true); });
            }
        }
        pause_key_active = true;
    }
    else
    {
        pause_key_active = false;
    }
}

void PingPongGame::stateHandler()
{
    switch (m_state)
    {
    case game_state::paused:
    {
        m_textState.setString("Paused");
        m_textLive.setString("Lives: " + std::to_string(m_live));

        aligning::Aligning(&m_textState, sf::FloatRect{0, 0, constants::window_width, constants::window_height});
        aligning::Aligning(&m_textLive, sf::FloatRect{0, 100, constants::window_width, constants::window_height - 100});
    }
    break;
    case game_state::game_over:
    {
        // databaseResultUpdate(m_state);
        m_textState.setString("Game Over");
        aligning::Aligning(&m_textState, sf::FloatRect{0, 0, constants::window_width, constants::window_height});
    }
    break;
    case game_state::player_wins:
    {
        // databaseResultUpdate(m_state);
        m_textState.setString("Win");
        aligning::Aligning(&m_textState, sf::FloatRect{0, 0, constants::window_width, constants::window_height});
    }
    break;
    case game_state::running:
        [[fallthrough]];
    default:
        break;
    }
}

/// @brief updating graphics
void PingPongGame::update()
{
    if (m_state == game_state::running)
    {
        //! Calculate the updated graphics
        m_entity_manager.update();

        //! Calculate interaction between the balls and the paddle
        //

        {
            std::set<std::pair<const paddle* const, ball* const>> handled_pairs;
            std::mutex handled_pairs_mutex;
            auto ball_paddle_interaction = [&handled_pairs, &handled_pairs_mutex](const paddle& a_paddle,
                                                                                  ball& a_ball) {
                {
                    const std::unique_lock<std::mutex> lock(handled_pairs_mutex);
                    if (handled_pairs.find(std::make_pair(&a_paddle, &a_ball)) != handled_pairs.end())
                    {
                        return;
                    }
                }
                handled_pairs.insert(std::make_pair(&a_paddle, &a_ball));
                interactions::BallvsPaddle handler(a_ball, a_paddle);
                handler();
            };

            auto fut_a = std::async(std::launch::async, [this, &ball_paddle_interaction]() {
                m_entity_manager.apply_all<paddle>([this, &ball_paddle_interaction](const paddle& a_paddle) {
                    m_entity_manager.apply_all<ball>([&a_paddle, &ball_paddle_interaction](ball& a_ball) {
                        ball_paddle_interaction(a_paddle, a_ball);
                    });
                });
            });

            auto fut_b = std::async(std::launch::async, [this, &ball_paddle_interaction]() {
                m_entity_manager.apply_all<ball>([this, &ball_paddle_interaction](ball& a_ball) {
                    m_entity_manager.apply_all<paddle>([&a_ball, &ball_paddle_interaction](const paddle& a_paddle) {
                        ball_paddle_interaction(a_paddle, a_ball);
                    });
                });
            });

            try
            {
                fut_a.get();
                fut_b.get();
            }
            catch (const std::exception& e)
            {
                std::cerr << "Exception in ball-paddle interaction: " << e.what() << '\n';
            }
            catch (...)
            {
                std::cerr << "Unknown exception in ball-paddle interaction\n";
            }
        }

        //! Calculate interaction between the balls and the wall

        // container for pairs of position and velocity parameters for creating new balls
        std::forward_list<std::pair<sf::Vector2f, sf::Vector2f>> clone_balls;
        m_entity_manager.apply_all<ball>([this, &clone_balls](ball& a_ball) mutable {
            m_entity_manager.apply_all<wall>([this, &a_ball, &clone_balls](wall& a_wall) mutable {
                decltype(ball().get_scale_status()) prev_scale_state{};
                auto suff_scale_state = prev_scale_state;

                prev_scale_state = a_ball.get_scale_status();
                interactions::BallVsWall handler(a_wall, a_ball);
                handler();
                suff_scale_state = a_ball.get_scale_status();

                while (!a_ball.clone_balls.empty())
                {
                    clone_balls.push_front(a_ball.clone_balls.front());
                    a_ball.clone_balls.pop_front();
                }
            });
        });

        //! Creating new balls
        while (!clone_balls.empty())
        {
            auto& [pos, vel] = clone_balls.front();

            m_entity_manager.create<ball>(pos.x, pos.y);
            auto* a_ball = dynamic_cast<ball*>(m_entity_manager.get_all<ball>().back());
            if (a_ball != nullptr)
            {
                a_ball->set_velocity(vel);
            }

            clone_balls.pop_front();
        }

        //! Update point
        m_point = 0;
        auto& walls = m_entity_manager.get_all<wall>();
        for (auto& a_wall : walls)
        {
            auto* const wptr = dynamic_cast<wall*>(a_wall);
            m_point += wptr->get_point();
        }

        //! Checking if it's time out
        if (m_countingText.is_timeout())
        {
            m_state = game_state::game_over;
            m_countingText.pause();
            return;
        }

        m_entity_manager.refresh();
        check_finish_by_ball();
        check_finish_by_wall();
    }
}

void PingPongGame::check_finish_by_ball()
{
    if (m_entity_manager.get_all<ball>().empty())
    {
        --m_live;
        if (m_live > 0)
        {
            m_entity_manager.apply_all<paddle>([](paddle& a_paddle) {
                a_paddle.init(constants::window_width / 2.0F, constants::window_height * 1.0F);
            });
            m_entity_manager.create<ball>(constants::window_width / 2.0F, constants::window_height / 2.0F);
            m_countingText.pause();
            m_state = game_state::paused;
        }
        else
        {
            m_state = game_state::game_over;
            m_countingText.pause();
        }
    }
}

void PingPongGame::check_finish_by_wall()
{
    if (m_entity_manager.get_all<wall>().empty())
    {
        m_state = game_state::player_wins;
        m_entity_manager.apply_all<ball>([](ball& a_ball) { a_ball.stop(); });
        m_countingText.pause();
    }
}

void PingPongGame::render()
{
    m_entity_manager.draw(game_window);
    if (m_state != game_state::running)
    {
        game_window.draw(m_textState);
    }
    if (m_state == game_state::paused)
    {
        game_window.draw(m_textLive);
    }
    m_textPoint.setString(to_string(m_point));
    game_window.draw(m_textPoint);
    game_window.draw(m_countingText);
    game_window.display();
}

void PingPongGame::try_createwall()
{
    try
    {
        m_entity_manager.create<wall>();
        m_entity_manager.apply_all<wall>([](wall& a_wall) {
            utilities::wallhelper::build_wall(a_wall, (constants::resouces_path + "wall.csv").c_str());
        });
    }
    catch (const std::ios::failure& e)
    {
        std::cerr << "terminate by ios::failure\n";
        std::cerr << e.what() << '\n';
        clear();
        return;
    }
    catch (const std::exception& e)
    {
        std::cerr << "terminate by exception\n";
        std::cerr << e.what() << '\n';
        clear();
        return;
    }
    catch (...)
    {
        std::cerr << "terminate by an unknown error\n";
        clear();
        return;
    }
}

void PingPongGame::try_login()
{
    try
    {
        std::pair<bool, std::string> result;
        {
            const auto window = make_unique<LoginWindow>();
            result = window->run();
        }
        if (result.first)
        {
            m_username = result.second;
            updateGameSessionID();
            updateGameNewHistory();
        }
        else
        {
            throw std::logic_error("User does not continue to login");
        }
    }
    catch (const std::exception& e)
    {
        cerr << "Connecting PingPong Game to database failed: " << e.what() << '\n';
        clear();
    }
}

void PingPongGame::initialize_text()
{
    using namespace utilities;

    m_textState.setFont(texthelper::getFont(CROSS_BOXED));
    m_textState.setString("Paused");
    m_textState.setCharacterSize(32);
    m_textState.setFillColor(sf::Color::Red);
    texthelper::aligning::Aligning(&m_textState,
                                   {0, 0, constants::window_width, constants::window_height},
                                   texthelper::aligning::MC);

    m_textLive.setFont(texthelper::getFont(CROSS_BOXED));
    m_textLive.setString("Lives: " + to_string(m_live));
    m_textLive.setCharacterSize(24);
    m_textLive.setFillColor(sf::Color(255, 26, 26));
    texthelper::aligning::Aligning(&m_textLive,
                                   {0, 100.0F, constants::window_width, constants::window_height - 100.0F},
                                   texthelper::aligning::MC);

    m_textPoint.setFont(texthelper::getFont(CROSS_BOXED));
    m_textPoint.setString("0");
    m_textPoint.setCharacterSize(24);
    m_textPoint.setFillColor(sf::Color::Magenta);
    m_textPoint.setPosition(0, 350);

    m_countingText.setFillColor(sf::Color::Blue);
    m_countingText.setPosition(0, 400);
    m_countingText.setFont(texthelper::getFont(CROSS_BOXED));
    m_countingText.setCharacterSize(24);
    m_countingText.set_limit(CountingText::duration{constants::round_duration});
}

PingPongGame::PingPongGame(std::string resourcePath)
    : m_live(constants::init_live), m_point(0), m_GameSessionID(0), savedData(false)
{
    PingPongGame::init(resourcePath);
}

PingPongGame::PingPongGame() : m_live(constants::init_live), m_point(0), m_GameSessionID(0), savedData(false)
{}

void PingPongGame::init(std::string& resourcePath)
{
    constants::resouces_path = resourcePath;

    // try_login();

    game_window.setFramerateLimit(60);
    game_window.setVerticalSyncEnabled(true);
    game_window.setPosition(sf::Vector2i{(1920 - constants::window_width) / 2, (1080 - constants::window_height) / 2});

    m_entity_manager.create<background>(0.0F, 0.0F);
    m_entity_manager.create<ball>(constants::window_width / 2.0F, constants::window_height / 2.0F);
    m_entity_manager.create<paddle>(constants::window_width / 2.0F, constants::window_height * 1.0F);

    try_createwall();

    initialize_text();
}

// Reinitialize the PingPongGame
void PingPongGame::reset()
{
    //! Reset the parameters
    m_state = game_state::running;
    m_live = constants::init_live;
    m_point = 0;

    //! Move a paddle to initial position
    m_entity_manager.apply_all<paddle>(
        [](paddle& a_paddle) { a_paddle.init(constants::window_width / 2.0f, constants::window_height * 1.0F); });

    //! Destroy the entities
    m_entity_manager.apply_all<ball>([](ball& a_ball) {
        a_ball.stop();
        a_ball.destroy();
    });
    m_entity_manager.apply_all<wall>([](wall& a_wall) { a_wall.destroy(); });

    //! Refresh
    m_entity_manager.refresh();

    //! Recreate
    m_entity_manager.create<ball>(constants::window_width / 2.0F, constants::window_height / 2.0F);
    try_createwall();

    //! Restart the counting text
    m_countingText.restart();
    m_countingText.stop_pause();
}

void PingPongGame::clear()
{
    m_entity_manager.clear();
    game_window.clear(sf::Color::Black);
    game_window.close();
}

// Game loop
void PingPongGame::run()
{
    try
    {
        m_countingText.start();
        while (game_window.isOpen())
        {
            game_window.clear(sf::Color::Black);
            listening();
            stateHandler();
            update();
            render();
        }
        m_countingText.stop();
        m_entity_manager.clear();
        ThreadPool::destroyInstance();
        SoundPlayer::destroyInstance();
        DBClient::DestroyInstance();
    }
    catch (const std::exception& e)
    {
        cerr << "Running PingPong Game failed: " << e.what() << '\n';
    }
}

extern "C" IGame* createPingPongGame()
{
    return new PingPongGame();
}

extern "C" void destroyGame(IGame* game)
{
    delete game;
    game = nullptr;
}
