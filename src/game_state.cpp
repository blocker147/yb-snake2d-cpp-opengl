#include "game_state.h"

namespace Snake2d {
	GameState::GameState() {}

	void GameState::initialize() {
		if (isInitialized)
			return;

		view = GameView::MAIN_MENU;
		isGameOver = false;
		score = 0;
		life = 3;

		int delayBetweenUpdatesMs = 100;
		int WORLD_HEIGHT = 20, WORLD_WIDTH = 20;

		worldTimer = new RepeatingTimer(0, 0, []() {}, delayBetweenUpdatesMs);
		world = new World(WORLD_HEIGHT, WORLD_WIDTH);

		isInitialized = true;
	}
	void GameState::cleanUp() {
		if (!isInitialized)
			return;

		delete worldTimer;
		worldTimer = nullptr;
		delete world;
		world = nullptr;

		isInitialized = false;
	}
	void GameState::update(UserInput::Direction direction) {
		uint64_t now = Clock::whatTimeIsIt();

		if (worldTimer->shouldUpdate(now)) {
			World::WorldUpdateInfo worldUpdateInfo;
			worldUpdateInfo = world->update(direction, now);

			this->life += worldUpdateInfo.lifeDelta;
			this->score += worldUpdateInfo.scoreDelta;
			this->isGameOver = this->life <= 0;
		}

		if (isGameOver)
			this->changeView(Snake2d::GameView::GAME_OVER_MENU);
	}
}