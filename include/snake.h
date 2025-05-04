#ifndef SNAKE2D_SNAKE
#define SNAKE2D_SNAKE

#include <deque>
#include <vector>
#include <iostream>

#include "game_object.h"

namespace Snake2d {
	class Snake {
	private:
		std::deque<SnakeBody*> body;
	public:
		Snake(std::deque<SnakeBody*> body);
		~Snake();

		std::deque<SnakeBody*> getBody();

		SnakeHead* getHead();
		SnakeBody* getNeck();
		SnakeBody* getBodyBeforeTail();
		SnakeTail* getTail();

		void increase(SnakeBody* neck, SnakeHead* head);
		void move(SnakeTail* tail, SnakeBody* neck, SnakeHead* head);

		std::vector<SnakeBody*> cutTailAt(SnakeHead* head);
	};
}

#endif