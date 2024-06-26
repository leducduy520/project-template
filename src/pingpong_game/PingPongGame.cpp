#include "PingPongGame.hpp"
#include "interactions.hpp"
#include "wallHelper.hpp"

std::string constants::resoucesPath;

void PingPongGame::eventHandler()
{
	static bool pause_key_active = false;
	static bool reset_key_active = false;
	sf::Event event;

	while (game_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			game_window.close();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
		game_window.close();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) {
		// If it was not pressed on the last iteration, toggle the status
		if (!pause_key_active) {
			if (m_state == game_state::paused)
				m_state = game_state::running;
			else
				m_state = game_state::paused;
		}
		pause_key_active = true;
	}
	else
		pause_key_active = false;

	// If the user presses "R", we reset the PingPongGame
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
	{
		if(!reset_key_active)
		{
            switch (m_state)
            {
            case PingPongGame::game_state::game_over:
                m_entity_manager.create<ball>();
                break;
            default:
                break;
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

void PingPongGame::handleGameState()
{
	switch (m_state)
    {
    case game_state::paused:
    {
        m_textState.setString("Paused");
		auto bound = m_textState.getLocalBounds();
		m_textState.setOrigin(bound.left + bound.width / 2.0f, bound.top + bound.height / 2.0f);
		m_textState.setPosition(game_window.getSize().x / 2.0f, game_window.getSize().y / 2.0f);
    }
    break;
    case game_state::game_over:
    {
        m_textState.setString("Game Over");
		auto bound = m_textState.getLocalBounds();
		m_textState.setOrigin(bound.left + bound.width / 2.0f, bound.top + bound.height / 2.0f);
		m_textState.setPosition(game_window.getSize().x / 2.0f, game_window.getSize().y / 2.0f);
    }
    break;
	case game_state::player_wins:
    {
        m_textState.setString("Win");
		auto bound = m_textState.getLocalBounds();
		m_textState.setOrigin(bound.left + bound.width / 2.0f, bound.top + bound.height / 2.0f);
		m_textState.setPosition(game_window.getSize().x / 2.0f, game_window.getSize().y / 2.0f);
    }
    break;
	default:
		m_textState.setString("");
        break;
    }
}

void PingPongGame::update()
{
	if (m_state == game_state::running)
	{
		// Calculate the updated graphics
		m_entity_manager.update();

		m_entity_manager.apply_all<ball>([this](ball& b){
			m_entity_manager.apply_all<paddle>([&b](const paddle& p){
				interactions::handle_interaction(b, p);
			});
		});

		m_entity_manager.apply_all<ball>([this](ball& b){
			m_entity_manager.apply_all<wall>([&b](wall& w){
				wall_utils::interactionwith<ball>(w, b);
                wall_utils::checkAlive(w);
			});
		});

		m_entity_manager.refresh();

		if(m_entity_manager.get_all<ball>().empty())
		{
			m_state = game_state::game_over;
		}

		if(m_entity_manager.get_all<wall>().empty())
		{
			m_state = game_state::player_wins;
            
		}
	}
}

void PingPongGame::render()
{
	m_entity_manager.draw(game_window);
	game_window.draw(m_textState);
	game_window.display();
}

void PingPongGame::try_createwall()
{
    try
    {
        wall w;
        wall_utils::createWall(w, (constants::resoucesPath + "wall.csv").c_str());
        m_entity_manager.create<wall>(std::move(w));
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

PingPongGame::PingPongGame(std::string resourcePath)
{
    init(resourcePath);
}

PingPongGame::PingPongGame()
{
}

void PingPongGame::init(std::string resourcePath)
{
	constants::resoucesPath = resourcePath;
	game_window.setFramerateLimit(60);
	game_window.setVerticalSyncEnabled(true);
    
	m_entity_manager.create<background>(0.0f, 0.0f);
	m_entity_manager.create<ball>(constants::window_width/2.0f, constants::window_height/2.0f);
	m_entity_manager.create<paddle>(constants::window_width/2.0f, constants::window_height * 1.0f);

    try_createwall();

	if(m_font.loadFromFile(constants::resoucesPath + "Cross Boxed.ttf"))
	m_textState.setFont(m_font);
	m_textState.setPosition(0, 0);
	m_textState.setCharacterSize(35);
    m_textState.setFillColor(sf::Color(255, 26, 26));
	m_textState.setString("");
}

// Reinitialize the PingPongGame
void PingPongGame::reset() {
	m_entity_manager.apply_all<ball>([](ball& b){b.init(constants::window_width/2.0f, constants::window_height/2.0f);});
	m_entity_manager.apply_all<paddle>([](paddle& b){b.init(constants::window_width/2.0f, constants::window_height * 1.0f);});
	m_entity_manager.apply_all<wall>([](wall& b){b.init(0.0f, 0.0f);});
}

void PingPongGame::clear()
{
    m_entity_manager.clear();
    game_window.clear(sf::Color::Black);
    game_window.close();
}

// Game loop
void PingPongGame::run() {
	while (game_window.isOpen()) {
        game_window.clear(sf::Color::Black);
		eventHandler();
		handleGameState();
		update();
		render();
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
