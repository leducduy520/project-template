#include "PingPongGame.hpp"
#include "DBClientGame.hpp"
#include "LoginGame.hpp"
#include "ThreadPoolGame.hpp"
#include "helper.hpp"
#include "interactions.hpp"
#include "soundplayer.hpp"

std::string constants::resoucesPath;
using namespace std;
using namespace std::literals;
using namespace utilities::texthelper;
using json = nlohmann::json;

extern void CountingTextUpdate(CountingText* text);

void PingPongGame::updateGameSessionStartTime()
{
    std::array<char, constants::fmtnow> buffer{};
    std::tm tmbuff{0};

#if defined(_WIN32) || defined(_WIN64)
    gmtime_s(&tmbuff, &m_GameSessionID);
#else
    gmtime_r(&m_GameSessionID, &tmbuff);
#endif

    strftime(buffer.data(), constants::fmtnow, "%F %T GMT", &tmbuff);

    DBINSTANCE->UpdateDocument(make_document(kvp("name", m_username), kvp("history.id", m_GameSessionID)),
                               make_document(kvp("$set", make_document(kvp("history.$.start_time", buffer.data())))));
}

std::string PingPongGame::toJsonString(const uint8_t* data, size_t length)
{
    bson_t bson;
    bson_init_static(&bson, data, length);

    size_t size;
    auto result = bson_array_as_json(&bson, &size);

    if (!result)
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

        std::array<char, constants::fmtnow> buffer{};
        std::tm tmbuff{0};

#if defined(_WIN32) || defined(_WIN64)
        gmtime_s(&tmbuff, &m_GameSessionID);
#else
        gmtime_r(&m_GameSessionID, &tmbuff);
#endif

        strftime(buffer.data(), constants::fmtnow, "%F %T GMT", &tmbuff);

        auto duration = minus<decltype(m_GameSessionID)>{}(m_GameSessionID, oldGameSessionID);

        DBINSTANCE->UpdateDocument(make_document(kvp("name", m_username), kvp("history.id", oldGameSessionID)),
                                   make_document(kvp("$set",
                                                     make_document(kvp("history.$.end_time", buffer.data()),
                                                                   kvp("history.$.duration", duration)))));
        updateGameRecord();
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
    DBINSTANCE->RunPipeLine(std::move(pipeline), std::move(opts));
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
        databaseResultUpdate(false);
    }
    updateGameNewHistory();
}

void PingPongGame::databaseResultUpdate(const bool& isWin)
{
    if (isWin)
    {
        DBINSTANCE->UpdateDocument(
            make_document(kvp("name", m_username), kvp("history.id", m_GameSessionID)),
            make_document(kvp("$set",
                              make_document(kvp("history.$.result", "win"),
                                            kvp("history.$.live", m_live),
                                            kvp("history.$.score",
                                                static_cast<int32_t>(m_point) + static_cast<int32_t>(20 * m_live))))));
    }
    else
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

void PingPongGame::try_database()
{}

void PingPongGame::listening()
{
    static bool pause_key_active = false;
    static bool reset_key_active = false;
    sf::Event event{};

    while (game_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            (m_state == game_state::running || m_state == game_state::paused) ? removeCurrentData()
                                                                              : updateGameSessionEndTime();
            game_window.close();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
    {
        (m_state == game_state::running || m_state == game_state::paused) ? removeCurrentData()
                                                                          : updateGameSessionEndTime();
        game_window.close();
    }

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

    // If the user presses "R", we reset the PingPongGame
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
    {
        if (!reset_key_active)
        {
            if (m_state == game_state::game_over)
            {
                m_entity_manager.create<ball>();
            }
            databaseRetryUpdate();
            reset();
        }
        reset_key_active = true;
    }
    else
    {
        reset_key_active = false;
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
        databaseResultUpdate(false);
        m_textState.setString("Game Over");
        aligning::Aligning(&m_textState, sf::FloatRect{0, 0, constants::window_width, constants::window_height});
    }
    break;
    case game_state::player_wins:
    {
        databaseResultUpdate(true);
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

void PingPongGame::update()
{
    if (m_state == game_state::running)
    {
        // Calculate the updated graphics
        m_entity_manager.update();

        m_entity_manager.apply_all<ball>([this](ball& a_ball) {
            m_entity_manager.apply_all<paddle>(
                [&a_ball](const paddle& a_paddle) { interactions::handle_interaction(a_ball, a_paddle); });
        });

        m_entity_manager.apply_all<ball>([this](ball& a_ball) {
            m_entity_manager.apply_all<wall>(
                [&a_ball](wall& a_wall) { interactions::handle_interaction(a_wall, a_ball); });
        });

        m_point = 0;
        auto& walls = m_entity_manager.get_all<wall>();
        for (auto& a_wall : walls)
        {
            auto* const wptr = dynamic_cast<wall*>(a_wall);
            m_point += utilities::wallhelper::getPoint(*wptr);
        }

        if (m_countingText.is_timeout())
        {
            m_entity_manager.apply_all<ball>([this](ball& a_ball) { a_ball.destroy(); });
            m_state = game_state::game_over;
            m_countingText.pause();
        }

        m_entity_manager.refresh();

        if (m_state == game_state::game_over)
        {
            return;
        }

        if (m_entity_manager.get_all<ball>().empty())
        {
            --m_live;
            if (m_live > 0)
            {
                m_entity_manager.create<ball>(constants::window_width / 2.0F, constants::window_height / 2.0F);
                m_entity_manager.apply_all<paddle>([](paddle& a_paddle) {
                    a_paddle.init(constants::window_width / 2.0F, constants::window_height * 1.0F);
                });
                m_state = game_state::paused;
            }
            else
            {
                m_state = game_state::game_over;
                m_countingText.pause();
            }
        }

        if (m_entity_manager.get_all<wall>().empty())
        {
            m_state = game_state::player_wins;
            m_entity_manager.apply_all<ball>([](ball& a_ball) { a_ball.stop(); });
            m_countingText.pause();
        }
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
    game_window.draw(m_countingText);
    game_window.display();
}

void PingPongGame::try_createwall()
{
    try
    {
        wall a_wall;
        m_entity_manager.create<wall>();
        m_entity_manager.apply_all<wall>([](wall& a_wall) {
            utilities::wallhelper::createWall(a_wall, (constants::resoucesPath + "wall.csv").c_str());
        });
    }
    catch (const std::ios::failure& e)
    {
        std::cerr << "terminate by ios::failure\n";
        std::cerr << e.what() << std::endl;
        clear();
        return;
    }
    catch (const std::exception& e)
    {
        std::cerr << "terminate by exception\n";
        std::cerr << e.what() << std::endl;
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

PingPongGame::PingPongGame(std::string resourcePath)
    : m_live(constants::init_live), m_point(0), m_GameSessionID(0), savedData(false)
{
    PingPongGame::init(resourcePath);
}

PingPongGame::PingPongGame() : m_live(constants::init_live), m_point(0), m_GameSessionID(0), savedData(false)
{}

void PingPongGame::init(std::string& resourcePath)
{
    constants::resoucesPath = resourcePath;

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

    game_window.setFramerateLimit(60);
    game_window.setVerticalSyncEnabled(true);
    game_window.setPosition(sf::Vector2i{(1920 - constants::window_width) / 2, (1080 - constants::window_height) / 2});

    m_entity_manager.create<background>(0.0F, 0.0F);
    m_entity_manager.create<ball>(constants::window_width / 2.0F, constants::window_height / 2.0F);
    m_entity_manager.create<paddle>(constants::window_width / 2.0F, constants::window_height * 1.0F);

    try_createwall();

    m_font.loadFromFile(constants::resoucesPath + "Cross Boxed.ttf");

    sf::Text textState("Paused", m_font, 32);
    textState.setFillColor(sf::Color(255, 26, 26));
    auto bound = textState.getLocalBounds();
    textState.setOrigin(bound.width / 2.0f, bound.height / 2.0f);
    textState.setPosition(constants::window_width / 2.0F, constants::window_height / 2.0F);

    sf::Text textLife("Lives: " + to_string(m_live), m_font, 26);
    textLife.setFillColor(sf::Color(255, 26, 26));
    bound = textLife.getLocalBounds();
    textLife.setOrigin(bound.width / 2.0f, bound.height / 2.0f);
    textLife.setPosition(constants::window_width / 2.0F, constants::window_height / 2.0F - 50.f);

    m_countingText.setString("ABC");
    m_countingText.setFillColor(sf::Color::Blue);
    m_countingText.setPosition(0, 400);
    m_countingText.setFont(m_font);
    m_countingText.setCharacterSize(24);
    m_countingText.setLimit(CountingText::duration{5min});
    m_countingText.start();

    m_textState = std::move(textState);
    m_textLive = std::move(textLife);
}

// Reinitialize the PingPongGame
void PingPongGame::reset()
{
    m_live = constants::init_live;
    m_point = 0;
    m_state = game_state::running;

    m_entity_manager.apply_all<paddle>(
        [](paddle& a_paddle) { a_paddle.init(constants::window_width / 2.0f, constants::window_height * 1.0F); });
    m_entity_manager.apply_all<ball>([](ball& a_ball) {
        a_ball.stop();
        a_ball.init(constants::window_width / 2.0F, constants::window_height / 2.0F);
    });

    m_entity_manager.apply_all<wall>([](wall& w) { w.destroy(); });
    m_entity_manager.refresh();
    try_createwall();
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
        SoundPlayer::getInstance();
        m_countingText_return = ThreadPool::getInstance()->submit(1, CountingTextUpdate, &m_countingText);
        while (game_window.isOpen())
        {
            game_window.clear(sf::Color::Black);
            listening();
            stateHandler();
            update();
            render();
        }
        m_countingText.stop();
        m_countingText_return.get();
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

/*
[
  {
    $group: {
      _id: null,
      password: {
        $first: "$password"
      },
      name: {
        $first: "$name"
      },
      worldrecord: {
        $push: "$history"
      }
    }
  },
  {
    $project: {
      _id: 0,
      password: 1,
      name: 1,
      worldrecord: {
        $slice: [
          {
            $sortArray: {
              input: {
                $reduce: {
                  input: "$worldrecord",
                  initialValue: [],
                  in: {
                    $concatArrays: [
                      "$$value",
                      "$$this"
                    ]
                  }
                }
              },
              sortBy: {
                score: -1,
                live: -1,
                duration: 1,
                id: 1
              }
            }
          },
          20
        ]
      }
    }
  }
]
*/
