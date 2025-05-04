#include "user_input.h"

namespace UserInput {
	Direction direction;

	bool isOpposite(Direction a, Direction b) {
		return 
			(a == Direction::UP && b == Direction::DOWN) ||
			(a == Direction::DOWN && b == Direction::UP) ||
			(a == Direction::LEFT && b == Direction::RIGHT) ||
			(a == Direction::RIGHT && b == Direction::LEFT);
	}

	bool updateDirection(Direction newDirection)
	{
		if (isOpposite(direction, newDirection) || direction == newDirection)
			return false;
		if (!isOpposite(direction, newDirection))
			direction = newDirection;
		return true;
	}
}
