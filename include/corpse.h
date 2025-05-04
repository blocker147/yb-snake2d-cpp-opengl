#ifndef SNAKE2D_SNAKE_CORPSE
#define SNAKE2D_SNAKE_CORPSE

#include <vector>
#include "game_object.h"

namespace Snake2d {
	class Corpse {
	private:
		std::vector<SnakeCorpse*> corpses;
	public:
		~Corpse();

		std::vector<SnakeCorpse*> getCorpses();

		void add(SnakeCorpse* corpse);

		void remove(std::vector<int> corpsesToRemove);
	};
}

#endif