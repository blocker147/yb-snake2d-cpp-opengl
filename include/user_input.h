#ifndef USER_INPUT_H
#define USER_INPUT_H

namespace UserInput {
	enum Direction { UP, DOWN, LEFT, RIGHT };

	extern Direction direction;

	bool updateDirection(Direction direction);
}

#endif