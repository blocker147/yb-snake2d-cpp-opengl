#include "corpse.h"

namespace Snake2d {
	Corpse ::~Corpse() {
		corpses.clear();
	}
	std::vector<SnakeCorpse*> Corpse::getCorpses() {
		return corpses;
	}
	void Corpse::add(SnakeCorpse* corpse) {
		corpses.push_back(corpse);
	}
	void Corpse::remove(std::vector<int> corpsesToRemove) {
		for (int targetIndex : corpsesToRemove)
			for (int i = 0; i < corpses.size(); i++)
				if (corpses[i]->index == targetIndex) {
					corpses.erase(corpses.begin() + i);
					break;
				}
	}
}
