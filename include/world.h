#ifndef SNAKE2D_WORLD
#define SNAKE2D_WORLD

#include <vector>
#include <random>

#include "game_object.h"
#include "snake.h"
#include "corpse.h"
#include "user_input.h"

namespace Snake2d {
	enum WorldConditionType { GENERATE_APPLE_LINE_SPAWNER };
	enum WorldConditionOperator { EQUAL };
	enum WorldConditionChangeOperator { INCREMENT };
	enum PostWorldConditionAction { RESET };
	/*
	* At the moment class used to specify conditions for World behavior.
	* When condition is met specific GameObject can be generated/spawned in world.
	* For example: "When '10' apples are eaten - then spawn 'AppleLineSpawner' GameObject in world."
	*/
	class WorldCondition {
	private:
		int initialValue, currentValue, expectedValue;
		WorldConditionType conditionType;
		WorldConditionOperator conditionOperator;
		PostWorldConditionAction postConditionAction;
	public:
		WorldCondition(
			int expectedValue,
			WorldConditionType conditionType,
			int initialValue = 0, WorldConditionOperator conditionOperator = EQUAL, PostWorldConditionAction postConditionAction = RESET
		) : expectedValue(expectedValue), conditionType(conditionType),
			initialValue(initialValue), currentValue(initialValue), conditionOperator(conditionOperator), postConditionAction(postConditionAction) {
		};

		bool isMet() const {
			bool isConditionMet = false;
			switch (conditionOperator) {
				case WorldConditionOperator::EQUAL: isConditionMet = currentValue == expectedValue; break;
			}
			return isConditionMet;
		}
		void changeCurrentValue(WorldConditionChangeOperator changeOperator, int byValue) {
			switch (changeOperator) {
				case Snake2d::INCREMENT: {
					currentValue += byValue;
					break;
				}
			}
		}
		void afterMet() {
			switch (postConditionAction) {
				case PostWorldConditionAction::RESET: currentValue = initialValue; break;
			}
		}
		WorldConditionType getType() const { return conditionType; }
	};

	class World {
	private:
		struct Coordinate { int x, y; };

		const int height, width;
		std::vector<std::vector<GameObject*>> field;
		Snake* snake;
		Corpse* corpse;

		// FIXME: move this variable to separate class (e.g GameState, GameConfig)
		int maxApplesAllowed = 1;

		void initializeField();
		void initializeSnake();
		void initializeDirection();

		int getIndex(Coordinate coordinate);
		Coordinate getCoordinate(int index);

		Coordinate getNextSnakeHead(UserInput::Direction direction, Coordinate coordinate);
		void swapDirectionIfHeadCollidesWithNeck(Coordinate snakeNeck, Coordinate nextSnakeHead);

		void generateApple();

		void setSnakeRotation();

		void assertOnlyOneHead();
		void assertOnlyOneTail();
		void assertSnake();
		// FIXME: do something with assertions
		void assertZeroApples();
		// FIXME: do something with assertions
		void assertOneApple();
		void assertPointers();
	public:
		// Holds world update results (e.g apple is eaten)
		struct WorldUpdateResult {
			GameObject::Type collisionWith;
			Coordinate collisionAt;
			int lifeDelta = 0;
			int scoreDelta = 0;
		};

		World(const int height, const int width);
		~World();

		// Generates GameObject by type at random world index that is empty (None)
		void generate(GameObject::Type type);

		WorldUpdateResult update(UserInput::Direction direction, uint64_t now);
		
		void afterUpdate(std::vector<WorldCondition*> postConditions);

		std::vector<std::vector<GameObject*>> getField() {
			return field;
		}
	};
}

#endif