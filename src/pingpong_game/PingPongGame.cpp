#include "PingPongGame.hpp"
#include "LoginGame.hpp"
#include "threadpool.hpp"
#include "helper.hpp"
#include "interactions.hpp"
#include "soundplayer.hpp"
#include "countingtext.hpp"
#include <memory>
#include <spdlog/spdlog.h>

std::filesystem::path constants::resouces_path;

extern void CountingTextUpdate(CountingText* text);

void PingPongGame::updateGameSessionStartTime()
{
    // No-op: MongoDB removed
}

std::string PingPongGame::toJsonString(const uint8_t*, size_t)
{
    return {};
}

nlohmann::json PingPongGame::toJson(const uint8_t*, size_t)
{
    return {};
}

void PingPongGame::updateGameSessionEndTime()
{
    if (!savedData) {
        auto oldGameSessionID = updateGameSessionID();

        std::array<char, constants::fmt_now> buffer{};
        std::tm tmbuff{0};

#if defined(_WIN32) || defined(_WIN64)
        gmtime_s(&tmbuff, &m_GameSessionID);
#else
        gmtime_r(&m_GameSessionID, &tmbuff);
#endif

        strftime(buffer.data(), constants::fmt_now, "%F %T GMT", &tmbuff);

        auto duration = std::minus<decltype(m_GameSessionID)>{}(m_GameSessionID, oldGameSessionID);

        savedData = true;
    }
}

void PingPongGame::updateGameRecord()
{
    // No-op: MongoDB removed
}

void PingPongGame::updateGameNewHistory()
{
    // No-op: MongoDB removed
    updateGameSessionStartTime();
    savedData = false;
}

int64_t PingPongGame::updateGameSessionID()
{
    return std::exchange(
        m_GameSessionID,
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

void PingPongGame::databaseRetryUpdate()
{
    if (m_state == game_state::running || m_state == game_state::paused) {
        databaseResultUpdate(game_state::game_over);
    }
    updateGameNewHistory();
}

void PingPongGame::databaseResultUpdate(const game_state&)
{
    // No-op: MongoDB removed
    updateGameSessionEndTime();
}

void PingPongGame::removeCurrentData()
{
    // No-op: MongoDB removed
}

void PingPongGame::listening()
{
    static bool pause_key_active = false;
    static bool reset_key_active = false;
    while (const auto event = game_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            game_window.close();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        game_window.close();
    }

    handleKeyPressed_P(pause_key_active);

    handleKeyPressed_R(reset_key_active);
}

void PingPongGame::handleKeyPressed_R(bool& reset_key_active)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
        if (!reset_key_active) {
            reset();
        }
        reset_key_active = true;
    }
    else {
        reset_key_active = false;
    }
}

void PingPongGame::handleKeyPressed_P(bool& pause_key_active)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) {
        // If it was not pressed on the last iteration, toggle the status
        if (!pause_key_active) {
            if (m_state == game_state::paused) {
                m_state = game_state::running;
                m_countingText.stop_pause();
                m_entity_manager->apply_all<BallCountingText>(
                    [](BallCountingText& a_coutningtext) { a_coutningtext.stop_pause(); });
            }
            else if (m_state == game_state::running) {
                m_state = game_state::paused;
                m_countingText.pause();
                m_entity_manager->apply_all<BallCountingText>(
                    [](BallCountingText& a_coutningtext) { a_coutningtext.pause(); });
            }
        }
        pause_key_active = true;
    }
    else {
        pause_key_active = false;
    }
}

void PingPongGame::stateHandler()
{
    using namespace utilities::texthelper;
    switch (m_state) {
    case game_state::paused:
    {
        m_textState.setString("Paused");
        m_textLive.setString("Lives: " + std::to_string(m_live));

        aligning::Aligning(&m_textState, sf::FloatRect{{0, 0}, {constants::window_width, constants::window_height}});
        aligning::Aligning(&m_textLive, sf::FloatRect{{0, 100}, {constants::window_width, constants::window_height - 100}});
    } break;
    case game_state::game_over:
    {
        m_textState.setString("Game Over");
        aligning::Aligning(&m_textState, sf::FloatRect{{0, 0}, {constants::window_width, constants::window_height}});
    } break;
    case game_state::player_wins:
    {
        m_textState.setString("Win");
        aligning::Aligning(&m_textState, sf::FloatRect{{0, 0}, {constants::window_width, constants::window_height}});
    } break;
    case game_state::running:
        [[fallthrough]];
    default:
        break;
    }
}

/// @brief updating graphics
void PingPongGame::update()
{
    if (m_state == game_state::running) {
        //! Calculate the updated graphics
        m_entity_manager->update();

        {
            std::vector<std::future<void>> futures;

            //! Calculate interaction between the balls and the paddle
            m_entity_manager->apply_all<paddle>([this, &futures](const paddle& a_paddle) {
                m_entity_manager->apply_all<ball>([&a_paddle, &futures](ball& a_ball) {
                    futures.emplace_back(ThreadPool::getInstance().submit(100, [&a_ball, &a_paddle]() {
                        interactions::BallvsPaddle handler(a_ball, a_paddle);
                        handler();
                    }));
                });
            });

            //! Calculate interaction between the balls and the wall
            m_entity_manager->apply_all<ball>([this, &futures](ball& a_ball) mutable {
                m_entity_manager->apply_all<wall>([this, &a_ball, &futures](wall& a_wall) mutable {
                    futures.emplace_back(ThreadPool::getInstance().submit(100, [this, &a_ball, &a_wall]() {
                        interactions::BallVsWall handler(a_wall, a_ball, *this->m_entity_manager);
                        handler();
                    }));
                });
            });

            for (auto& fut : futures) {
                fut.wait();
            }
        }

        //! Update point
        m_point = 0;
        const auto& walls = m_entity_manager->get_all<wall>();
        for (auto& a_wall : walls) {
            const auto* const wptr = dynamic_cast<wall*>(a_wall);
            m_point += wptr->getStatus().point;
        }

        //! Checking if it's time out
        if (m_countingText.is_timeout()) {
            m_state = game_state::game_over;
            m_countingText.pause();
            m_entity_manager->apply_all<BallCountingText>(
                [](BallCountingText& a_coutningtext) { a_coutningtext.pause(); });
            return;
        }

        m_entity_manager->refresh();
        check_finish_by_ball();
        check_finish_by_wall();
    }
}

void PingPongGame::check_finish_by_ball()
{
    if (m_entity_manager->get_all<ball>().empty()) {
        --m_live;
        if (m_live > 0) {
            m_entity_manager->apply_all<paddle>([](paddle& a_paddle) {
                a_paddle.init(constants::window_width / 2.0F, constants::window_height * 1.0F);
            });
            m_entity_manager->create<ball>(constants::window_width / 2.0F, constants::window_height / 2.0F);
            m_countingText.pause();
            m_entity_manager->apply_all<BallCountingText>(
                [](BallCountingText& a_coutningtext) { a_coutningtext.pause(); });
            m_state = game_state::paused;
        }
        else {
            m_state = game_state::game_over;
            m_countingText.pause();
            m_entity_manager->apply_all<BallCountingText>(
                [](BallCountingText& a_coutningtext) { a_coutningtext.pause(); });
        }
    }
}

void PingPongGame::check_finish_by_wall()
{
    if (m_entity_manager->get_all<wall>().empty()) {
        m_state = game_state::player_wins;
        m_countingText.pause();
        m_entity_manager->apply_all<BallCountingText>([](BallCountingText& a_coutningtext) { a_coutningtext.pause(); });
    }
}

void PingPongGame::render()
{
    game_window.clear(sf::Color::Black);
    m_entity_manager->draw(game_window);
    if (m_state != game_state::running) {
        game_window.draw(m_textState);
    }
    if (m_state == game_state::paused) {
        game_window.draw(m_textLive);
    }
    m_textPoint.setString(std::to_string(m_point));
    game_window.draw(m_textPoint);
    game_window.draw(m_countingText);
    game_window.display();
}

void PingPongGame::try_createwall()
{
    try {
        m_entity_manager->create<wall>();
        m_entity_manager->apply_all<wall>([](wall& a_wall) {
            a_wall.load_from_file(constants::resouces_path / "wall.csv");
        });
    }
    catch (const std::ios::failure& e) {
        spdlog::error("Game terminated due to I/O failure: {}", e.what());
        clear();
        return;
    }
    catch (const std::exception& e) {
        spdlog::error("Game terminated due to exception: {}", e.what());
        clear();
        return;
    }
    catch (...) {
        spdlog::critical("Game terminated due to unknown error");
        clear();
        return;
    }
}

void PingPongGame::try_login()
{
    try {
        std::pair<bool, std::string> result;
        {
            const auto window = std::make_unique<LoginWindow>();
            result = window->run();
        }
        if (result.first) {
            m_username = result.second;
            updateGameSessionID();
            updateGameNewHistory();
        }
        else {
            throw std::logic_error("User does not continue to login");
        }
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to connect PingPong Game to database: {}", e.what());
        clear();
    }
}

void PingPongGame::initialize_text()
{
    using namespace utilities;
    using namespace utilities::texthelper;

    m_textState.setFont(texthelper::getFont(CROSS_BOXED));
    m_textState.setString("Paused");
    m_textState.setCharacterSize(32);
    m_textState.setFillColor(sf::Color::Red);
    texthelper::aligning::Aligning(&m_textState,
                                   {{0, 0}, {constants::window_width, constants::window_height}},
                                   texthelper::aligning::MC);

    m_textLive.setFont(texthelper::getFont(CROSS_BOXED));
    m_textLive.setString("Lives: " + std::to_string(m_live));
    m_textLive.setCharacterSize(24);
    m_textLive.setFillColor(sf::Color(255, 26, 26));
    texthelper::aligning::Aligning(&m_textLive,
                                   {{0, 100.0F}, {constants::window_width, constants::window_height - 100.0F}},
                                   texthelper::aligning::MC);

    m_textPoint.setFont(texthelper::getFont(CROSS_BOXED));
    m_textPoint.setString("0");
    m_textPoint.setCharacterSize(24);
    m_textPoint.setFillColor(sf::Color::Magenta);
    m_textPoint.setPosition({0, 350});

    m_countingText.setFillColor(sf::Color::Blue);
    m_countingText.setPosition({0, 400});
    m_countingText.setFont(texthelper::getFont(CROSS_BOXED));
    m_countingText.setCharacterSize(24);
    m_countingText.set_limit(CountingText::duration{constants::round_duration});
}

PingPongGame::PingPongGame(std::string resourcePath)
{
    PingPongGame::init(resourcePath);
}

void PingPongGame::init(std::string& resourcePath)
{
    constants::resouces_path = resourcePath;

    game_window.setFramerateLimit(60);
    game_window.setVerticalSyncEnabled(true);
    game_window.setPosition(sf::Vector2i{(1920 - constants::window_width) / 2, (1080 - constants::window_height) / 2});

    m_entity_manager = std::make_shared<entity_manager>();
    m_entity_manager->create<background>(0.0F, 0.0F);
    m_entity_manager->create<ball>(constants::window_width / 2.0F, constants::window_height / 2.0F);
    m_entity_manager->create<paddle>(constants::window_width / 2.0F, constants::window_height * 1.0F);

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
    m_entity_manager->apply_all<paddle>(
        [](paddle& a_paddle) { a_paddle.init(constants::window_width / 2.0f, constants::window_height * 1.0F); });

    //! Destroy the entities
    m_entity_manager->apply_all<ball>([](ball& a_ball) { a_ball.destroy(); });
    m_entity_manager->apply_all<wall>([](wall& a_wall) { a_wall.destroy(); });
    m_entity_manager->apply_all<BallCountingText>([](BallCountingText& a_countingtext) { a_countingtext.destroy(); });

    //! Refresh
    m_entity_manager->refresh();

    //! Recreate
    m_entity_manager->create<ball>(constants::window_width / 2.0F, constants::window_height / 2.0F);
    try_createwall();

    //! Restart the counting text
    m_countingText.restart();
}

void PingPongGame::clear()
{
    m_entity_manager->clear();
    game_window.clear(sf::Color::Black);
    game_window.close();
}

// Game loop
void PingPongGame::run()
{
    constexpr size_t sound_pool_volume = 10;
    constexpr float sound_default_volume = 80.0f;
    constexpr size_t thread_pool_volume = 8;
    try {
        SoundManager::getInstance(sound_pool_volume, sound_default_volume);
        ThreadPool::getInstance(thread_pool_volume);

        SoundPlayer::loadSounds();
        m_countingText.start();
        while (game_window.isOpen()) {
            listening();
            stateHandler();
            update();
            render();
        }
        m_countingText.stop();
        SoundPlayer::stopSounds();
        ThreadPool::getInstance().wait();
        m_entity_manager->clear();
    }
    catch (const std::exception& e) {
        spdlog::critical("Failed to run PingPong Game: {}", e.what());
    }
}

extern "C" IGame* createPingPongGame()
{
    // Use std::make_unique for exception-safe allocation
    // Then release() to transfer ownership to the caller (C interface contract)
    // The caller is responsible for deleting the returned pointer
    return std::make_unique<PingPongGame>().release();
}
