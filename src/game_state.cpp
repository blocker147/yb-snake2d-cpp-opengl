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

		// Once 7 Apple's are eaten World should generate AppleLineSpawner & reset condition back to 0
		WorldCondition* whenToGenerateAppleLineSpawnerCondition = new WorldCondition(
			7, WorldConditionType::GENERATE_APPLE_LINE_SPAWNER
		);
		// Once 10 SnakeCorpse's are eaten World should generate SnakeBoneDestroyer & reset condition back to 0
		WorldCondition* whenToGenerateSnakeBoneDestroyer = new WorldCondition(
			10, WorldConditionType::GENERATE_SNAKE_BONE_DESTROYER
		);
		postWorldConditions[WorldConditionType::GENERATE_APPLE_LINE_SPAWNER] = whenToGenerateAppleLineSpawnerCondition;
		postWorldConditions[WorldConditionType::GENERATE_SNAKE_BONE_DESTROYER] = whenToGenerateSnakeBoneDestroyer;

		isInitialized = true;
	}
	void GameState::cleanUp() {
		if (!isInitialized)
			return;

		delete worldTimer;
		worldTimer = nullptr;
		delete world;
		world = nullptr;
		
		for (auto& [_, worldCondition] : postWorldConditions) {
			delete worldCondition;
			worldCondition = nullptr;
		}
		postWorldConditions.clear();

		isInitialized = false;
	}
	void GameState::update(UserInput::Direction direction) {
		uint64_t now = Clock::whatTimeIsIt();

		if (worldTimer->shouldUpdate(now)) {
			World::WorldUpdateResult worldUpdateResult;
			worldUpdateResult = world->update(direction, now);

			this->life += worldUpdateResult.lifeDelta;
			this->score += worldUpdateResult.scoreDelta;
			this->isGameOver = this->life <= 0;

			// Increment value by '1' when apple is eaten
			if (worldUpdateResult.collisionWith == GameObject::Type::APPLE) {
				postWorldConditions[WorldConditionType::GENERATE_APPLE_LINE_SPAWNER]->changeCurrentValue(
					WorldConditionChangeOperator::INCREMENT, 1
				);
			}
			else if (worldUpdateResult.collisionWith == GameObject::Type::SNAKE_CORPSE) {
				postWorldConditions[WorldConditionType::GENERATE_SNAKE_BONE_DESTROYER]->changeCurrentValue(
					WorldConditionChangeOperator::INCREMENT, 1
				);
			}

			std::vector<WorldCondition*> postWorldConditionsValues;
			for (auto& [_, worldCondition] : postWorldConditions)
				postWorldConditionsValues.push_back(worldCondition);
			world->afterUpdate(postWorldConditionsValues);
		}

		if (isGameOver)
			this->changeView(Snake2d::GameView::GAME_OVER_MENU);
	}
}