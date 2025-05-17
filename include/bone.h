#ifndef SNAKE2D_SNAKE_BONE
#define SNAKE2D_SNAKE_BONE

#include <vector>
#include "game_object.h"

namespace Snake2d {
	class Bone {
	private:
		std::vector<SnakeBone*> bones;
	public:
		~Bone() {
			bones.clear();
		}

		std::vector<SnakeBone*> getBones() const { return bones; }

		void add(SnakeBone* bone) {
			bones.push_back(bone);
		}

		void remove(std::vector<int> bonesToRemove) {
			for (int targetIndex : bonesToRemove)
				for (int i = 0; i < bones.size(); i++)
					if (bones[i]->index == targetIndex) {
						bones.erase(bones.begin() + i);
						break;
					}
		}

		void clear() {
			bones.clear();
		}
	};
}

#endif