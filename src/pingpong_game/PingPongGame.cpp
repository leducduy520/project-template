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
			reset();
		}
		reset_key_active = true;
	}
	else
	{
		reset_key_active = false;
	}

}

void PingPongGame::update()
{
	if (m_state != game_state::paused)
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
			});
		});

		// m_entity_manager.refresh();
	}
}

void PingPongGame::render()
{
	game_window.clear(sf::Color::Black);

	m_entity_manager.draw(game_window);

	game_window.display();
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

    

	m_entity_manager.create<background>(0.0f, 0.0f);
	m_entity_manager.create<ball>(constants::window_width/2.0f, constants::window_height/2.0f);
	m_entity_manager.create<paddle>(constants::window_width/2.0f, constants::window_height * 1.0f);

	/*wall w;
    wall_utils::createWall(w, (constants::resoucesPath + "wall.csv").c_str());
	m_entity_manager.create<wall>(std::move(w));*/

	

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

// Reinitialize the PingPongGame
void PingPongGame::reset() {
	game_window.clear(sf::Color::Black);
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
		eventHandler();
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
