#ifndef GAME_H
#define GAME_H

#include "background.hpp"
#include "ball.hpp"
#include "brick.hpp"
#include "constants.hpp"
#include "paddle.hpp"
#include "IGame.h"
#include "entityManager.hpp"

class PingPongGame : public IGame {
	
	sf::RenderWindow game_window{{constants::window_width, constants::window_height},
		"Simple Breakout Game Version 9"};
	entity_manager m_entity_manager;
	enum class game_state { paused, running };
	game_state m_state{game_state::running};

	void eventHandler();
	void update();
	void render();

public:
	PingPongGame(std::string resourcePath);
	PingPongGame();
	void init(std::string resourcePath) override;
	// Reinitialize the game
	void reset();

	void clear();

	// Game loop
	void run() override;
};

#endif // GAME_H
