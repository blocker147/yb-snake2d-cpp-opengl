#ifndef SNAKE2D_WORLD
#define SNAKE2D_WORLD

#include <vector>
#include <random>

#include "game_object.h"
#include "snake.h"
#include "corpse.h"
#include "user_input.h"

namespace Snake2d {
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
		struct WorldUpdateInfo {
			int lifeDelta = 0;
			int scoreDelta = 0;
		};

		World(const int height, const int width);
		~World();

		WorldUpdateInfo update(UserInput::Direction direction, uint64_t now);

		std::vector<std::vector<GameObject*>> getField() {
			return field;
		}
	};
}

#endif