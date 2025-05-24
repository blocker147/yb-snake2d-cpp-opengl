#ifndef SNAKE2D_GAME_STATE_H
#define SNAKE2D_GAME_STATE_H

#include <vector>
#include <optional>

#include "game_object.h"
#include "world.h"
#include "user_input.h"
#include "clock.h"

namespace Snake2d {

	enum GameView { MAIN_MENU, PLAYING_GAME_MENU, GAME_OVER_MENU };

	class GameState {
	private:
		bool isInitialized;

		GameView view;
		bool isGameOver;

		RepeatingTimer* worldTimer;
		World* world;
		int score;
		int life;

		// Holds unique game conditions that are used during game
		std::map<WorldConditionType, WorldCondition*> postWorldConditions;

		void initializePostWorldConditions();
	public:
		GameState();

		void initialize();
		void cleanUp();
		std::optional<World::WorldUpdateResult> update(UserInput::Direction direction);

		void changeView(GameView view) { this->view = view; }
		GameView getView() const { return this->view; }
		bool getIsGameOver() const { return isGameOver; }
		std::vector<std::vector<GameObject*>> getWorld() { return world->getField(); }
		int getScore() const { return score; }
		int getLife() const { return life; }
	};
}

#endif