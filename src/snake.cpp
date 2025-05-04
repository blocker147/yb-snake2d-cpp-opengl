#include "snake.h"

namespace Snake2d {
	Snake::Snake(std::deque<SnakeBody*> body) : body(body) {}

	Snake::~Snake() {
		for (int i = 0; i < body.size(); i++)
			body[i] = nullptr;
		body.clear();
	}

	std::deque<SnakeBody*> Snake::getBody() {
		return body;
	}

	SnakeHead* Snake::getHead() {
		return dynamic_cast<SnakeHead*>(body.front());
	}

	SnakeBody* Snake::getNeck() {
		return body[1];
	}

	SnakeBody* Snake::getBodyBeforeTail() {
		return body[body.size() - 2];
	}

	SnakeTail* Snake::getTail() {
		return dynamic_cast<SnakeTail*>(body.back());
	}

	void Snake::increase(SnakeBody* neck, SnakeHead* head) {
		body.pop_front();
		body.push_front(neck);
		body.push_front(head);

		for (int i = 0; i < body.size(); i++)
			if (body[i]->index < 0 || body[i]->index > 1000)
				std::cout << "Pointer to garbage found in snake body";
	}

	void Snake::move(SnakeTail* tail, SnakeBody* neck, SnakeHead* head) {
		body.pop_back();
		body.pop_back();
		body.push_back(tail);

		body.pop_front();
		body.push_front(neck);
		body.push_front(head);

		for (int i = 0; i < body.size(); i++)
			if (body[i]->index < 0 || body[i]->index > 100000)
				std::cout << "Pointer to garbage found in snake body";
	}

	std::vector<SnakeBody*> Snake::cutTailAt(SnakeHead* head) {
		std::vector<SnakeBody*> cutOffSnakeBody;
		
		int cutBodyAtIndex{};
		bool foundWhereToCutOff{};
		for (int i = 1; i < body.size(); i++)
		{
			if (foundWhereToCutOff) {
				cutOffSnakeBody.push_back(body[i]);
				continue;
			}
			if (body[i]->index == head->index) {
				foundWhereToCutOff = true;
				cutBodyAtIndex = i;
			}
		}

		// TODO: usually snake is updated after memory manipulation in WORLD
		auto start = body.begin() + cutBodyAtIndex;
		auto end = body.end();
		body.erase(start, end);

		return cutOffSnakeBody;
	}
}