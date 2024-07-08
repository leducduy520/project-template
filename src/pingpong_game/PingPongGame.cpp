#include "PingPongGame.hpp"
#include "interactions.hpp"
#include "wallHelper.hpp"
#include "soundplayer.hpp"

std::string constants::resoucesPath;
using namespace std;

void PingPongGame::eventHandler()
{
	static bool pause_key_active = false;
	static bool reset_key_active = false;
	sf::Event event{};

	while (game_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
        {
            game_window.close();
        }
    }

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
    {
        game_window.close();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) 
	{
        // If it was not pressed on the last iteration, toggle the status
        if (!pause_key_active) {
			if (m_state == game_state::paused)
            {
                m_state = game_state::running;
            }
			else
			{
                m_state = game_state::paused;
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
		if(!reset_key_active)
		{
			if (m_state == game_state::game_over)
			{
                m_entity_manager.create<ball>();
			}
			reset();
			m_state = game_state::running;
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

        centeredText(m_textState);
    }
    break;
    case game_state::game_over:
    {
        m_textState.setString("Game Over");
        centeredText(m_textState);
    }
    break;
    case game_state::player_wins:
    {
        m_textState.setString("Win");
        centeredText(m_textState);
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

		m_entity_manager.apply_all<ball>([this](ball& a_ball){
			m_entity_manager.apply_all<paddle>([&a_ball](const paddle& a_paddle){
				interactions::handle_interaction(a_ball, a_paddle);
			});
		});

		m_entity_manager.apply_all<ball>([this](ball& a_ball){
			m_entity_manager.apply_all<wall>([&a_ball](wall& a_wall){
				wall_utils::interactionwith<ball>(a_wall, a_ball);
                wall_utils::checkAlive(a_wall);
			});
		});

		m_entity_manager.refresh();

		if(m_entity_manager.get_all<ball>().empty())
		{
			--m_live;
            m_entity_manager.create<ball>(constants::window_width / 2.0F, constants::window_height / 2.0F);
            m_entity_manager.apply_all<paddle>(
                [](paddle &a_paddle) { a_paddle.init(constants::window_width / 2.0F, constants::window_height * 1.0F); });
            m_state = game_state::paused;
		}

		if(m_entity_manager.get_all<wall>().empty())
		{
			m_state = game_state::player_wins;
            
		}

        if (m_live == 0)
        {
            m_state = game_state::game_over;
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
    game_window.display();
}

void PingPongGame::try_createwall()
{
    try
    {
        wall a_wall;
        wall_utils::createWall(a_wall, (constants::resoucesPath + "wall.csv").c_str());
        m_entity_manager.create<wall>(std::move(a_wall));
    }
    catch (const std::ios::failure &e)
    {
        std::cerr << "terminate by ios::failure\n";
        clear();
        return;
    }
    catch (const std::exception &e)
    {
        std::cerr << "terminate by exception\n";
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

void PingPongGame::centeredText(sf::Text &text)
{
    auto textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(constants::window_width / 2.0f, constants::window_height / 2.0f);
}

PingPongGame::PingPongGame(std::string resourcePath) : m_live(3)
{
    PingPongGame::init(resourcePath);
}

PingPongGame::PingPongGame() : m_live(3)
{
}

void PingPongGame::init(std::string& resourcePath)
{
	constants::resoucesPath = resourcePath;
	game_window.setFramerateLimit(60);
	game_window.setVerticalSyncEnabled(true);
    
	m_entity_manager.create<background>(0.0F, 0.0F);
	m_entity_manager.create<ball>(constants::window_width/2.0F, constants::window_height/2.0F);
	m_entity_manager.create<paddle>(constants::window_width/2.0F, constants::window_height * 1.0F);

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

    m_textState = std::move(textState);
    m_textLive = std::move(textLife);
}

// Reinitialize the PingPongGame
void PingPongGame::reset() {
    m_live = 3;
	m_entity_manager.apply_all<ball>([](ball& a_ball){a_ball.init(constants::window_width/2.0f, constants::window_height/2.0f);});
	m_entity_manager.apply_all<paddle>([](paddle& a_paddle){a_paddle.init(constants::window_width/2.0f, constants::window_height * 1.0f);});
    try_createwall();
}

void PingPongGame::clear()
{
    m_entity_manager.clear();
    game_window.clear(sf::Color::Black);
    game_window.close();
}

// Game loop
void PingPongGame::run() {
	SoundPlayer::getInstance();
	while (game_window.isOpen()) {
        game_window.clear(sf::Color::Black);
		eventHandler();
		stateHandler();
		update();
		render();
	}
    SoundPlayer::destroyInstance();
    m_entity_manager.clear();
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
