#ifndef SNAKE2D_WORLD
#define SNAKE2D_WORLD

#include <vector>
#include <random>

#include "game_object.h"
#include "snake.h"
#include "corpse.h"
#include "bone.h"
#include "user_input.h"

namespace Snake2d {
	enum WorldConditionType {
		GENERATE_APPLE,
		GENERATE_APPLE_LINE_SPAWNER,
		GENERATE_SNAKE_BONE_DESTROYER
	};
	enum WorldConditionOperator {
		EQUAL,
		// LT = Less Than
		LT
	};
	enum WorldConditionChangeOperator {
		SET,
		INCREMENT
	};
	enum PostWorldConditionAction { RESET };
	/*
	* At the moment class used to specify conditions for World behavior.
	* When condition is met specific GameObject can be generated/spawned in world.
	* For example: "When '10' apples are eaten - then spawn 'AppleLineSpawner' GameObject in world."
	* 
	* Checklist when creating new WorldCondition from new WorldConditionType:
	* - add new type in WorldConditionType
	* - add new instance of WorldCondition in gameState.cpp in initialize method
	* - update gameState.cpp to include logic when WorldUpdateResult is received after world.update()
	* - update world.afterUpdate() method
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
				case WorldConditionOperator::EQUAL: isConditionMet = currentValue == expectedValue;		break;
				case WorldConditionOperator::LT:	isConditionMet = currentValue < expectedValue;		break;
			}
			return isConditionMet;
		}
		void changeCurrentValue(WorldConditionChangeOperator changeOperator, int value) {
			switch (changeOperator) {
				case WorldConditionChangeOperator::SET:			currentValue = value;	break;
				case WorldConditionChangeOperator::INCREMENT:	currentValue += value;	break;
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
		Bone* bone;

		void initializeField();
		void initializeSnake();
		void initializeCorpse();
		void initializeBone();
		void initializeDirection();

		int getIndex(Coordinate coordinate);
		Coordinate getCoordinate(int index);

		Coordinate getNextSnakeHead(UserInput::Direction direction, Coordinate coordinate);
		void swapDirectionIfHeadCollidesWithNeck(Coordinate snakeNeck, Coordinate nextSnakeHead);

		void setSnakeRotation();

		/* Generates GameObject by type at random world index that is empty(None) */
		void generate(GameObject::Type type);
		/* Counts all objects of specified type in world*/
		int count(GameObject::Type type);

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

			int applesInWorld = 0;
		};

		World(const int height, const int width);
		~World();

		WorldUpdateResult update(UserInput::Direction direction, uint64_t now);
		
		void afterUpdate(std::vector<WorldCondition*> postConditions);

		std::vector<std::vector<GameObject*>> getField() { return field; }
	};
}

#endif