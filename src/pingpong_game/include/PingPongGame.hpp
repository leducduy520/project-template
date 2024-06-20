#ifndef GAME_H
#define GAME_H

#include "background.hpp"
#include "ball.hpp"
#include "brick.hpp"
#include "constants.hpp"
#include "paddle.hpp"
#include "../../interface/IGame.h"

class PingPongGame : public IGame {
	// Create the game's window using an object of class RenderWindow
	// The constructor takes an SFML 2D vector with the window dimensions
	// and an std::string with the window title
	// The SFML code is in the sf namespace
	sf::RenderWindow game_window{{constants::window_width, constants::window_height},
		"Simple Breakout Game Version 9"};

	// Create the background object
	background the_background;

	// Create a ball object in the middle of the screen
	ball the_ball;

	// Create a paddle object at the bottom of the screen in the middle
	paddle the_paddle;

	// Create the grid of bricks
	// We will use an std::vector to store them
	wall bricks;

	std::string m_resourcesPath;

	// Enum with allowed values for the game's state
	enum class game_state { paused, running };

	// Member to store the current state of the game
	game_state state{game_state::running};

	void eventHandler();
	void update();
	void render();

public:
	PingPongGame(std::string resourcePath);
	PingPongGame();
	void init(std::string resourcePath) override;
	// Reinitialize the game
	void reset();

	// Game loop
	void run() override;
};

#endif // GAME_H
