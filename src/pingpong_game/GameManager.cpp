#include "PingPongGame.hpp"
#include "interactions.hpp"

std::string constants::resoucesPath;

void PingPongGame::eventHandler()
{
	static bool pause_key_active = false;
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
			if (state == game_state::paused)
				state = game_state::running;
			else
				state = game_state::paused;
		}
		pause_key_active = true;
	}
	else
		pause_key_active = false;

	// If the user presses "R", we reset the PingPongGame
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
		reset();
}

void PingPongGame::update()
{
	if (state != game_state::paused) 
	{
		// Calculate the updated graphics
		the_background.update();
		the_ball.update();
		the_paddle.update();

		handle_interaction(the_ball, the_paddle);

		// Check every brick for a collision with the ball
		bricks.erase(std::remove_if(std::begin(bricks), std::end(bricks),
				[this](brick& b) { handle_interaction(the_ball, b); return b.is_destroyed(); }),
		std::end(bricks));
	}
}

void PingPongGame::render()
{
	game_window.clear(sf::Color::Black);
	the_background.draw(game_window);
	the_ball.draw(game_window);
	the_paddle.draw(game_window);
	
	for (auto b: bricks)
	{
		b.draw(game_window);
	}

	game_window.display();   
}

PingPongGame::PingPongGame(std::string resourcePath) 
	: m_resourcesPath{resourcePath}
{
    init(m_resourcesPath);
}

PingPongGame::PingPongGame()
{
}

void PingPongGame::init(std::string resourcePath)
{
	constants::resoucesPath = resourcePath;
	game_window.setFramerateLimit(60);
	the_background.init(0.0f, 0.0f); 
	the_ball.init(constants::window_width/2.0f, constants::window_height/2.0f);
	the_paddle.init(constants::window_width/2.0f, constants::window_height);
	reset();
}

// Reinitialize the PingPongGame
void PingPongGame::reset() {
	game_window.clear(sf::Color::Black);
	bricks.clear();
    for (int i = 0; i < constants::brick_verical_lanes; ++i) {
		for (int j = 0; j < constants:: brick_horizontal_lanes; ++j) {
			// Calculate the brick's position
			float x = constants::brick_padding + i * constants::brick_width;
			float y = j * constants::brick_height;

			// Create the brick object
			bricks.emplace_back(x, y);
		}
	}
	the_ball.set_position({constants::window_width/2.0f, constants::window_height/2.0f});
	the_paddle.set_position({constants::window_width/2.0f, constants::window_height});
}

// Game loop
void PingPongGame::run() {
	// Was the pause key pressed in the last frame?
	
	while (game_window.isOpen()) {
		

		eventHandler();

		// In the paused state, the entities are not updated, only redrawn
		update();
		
		// Display the updated graphics
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