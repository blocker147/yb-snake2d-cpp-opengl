#include "clock.h"
#include <iostream>

namespace Snake2d {
	Clock::Clock(uint64_t id, uint64_t startedAt, std::function<void()> consumer) : id(id), startedAt(startedAt), consumer(consumer) {}
	uint64_t Clock::getId() const {
		return id;
	}
	void Clock::consume() const {
		consumer();
	}

	RepeatingTimer::RepeatingTimer(uint64_t id, uint64_t startedAt, std::function<void()> consumer, uint64_t delay) : Clock::Clock(id, startedAt, consumer), delay(delay), times(0), updatedAt(startedAt), isInfinite(true) {}
	// TODO: not the best solution to allow player to change direction
	bool RepeatingTimer::update(const uint64_t& now, bool force) {
		if ((force && now - updatedAt >= 70) || now - updatedAt >= delay) {
			uint64_t passed = now - updatedAt;
			//std::cout << passed << "ms. " << std::boolalpha << force << std::endl;
			consume();
			updatedAt = now;
			if (!isInfinite)
				times--;
		}
		return !isInfinite && times == 0;
	}
	bool RepeatingTimer::shouldUpdate(const uint64_t& now, bool force) {
		if ((force && now - updatedAt >= 70) || now - updatedAt >= delay) {
			uint64_t passed = now - updatedAt;
			updatedAt = now;
			if (!isInfinite)
				times--;
			return true;
		}
		return false;
	}

	ClockManager::~ClockManager() {
		for (auto& [id, clock] : clocks) {
			delete clock;
			clock = nullptr;
		}
		clocks.clear();
	}
	void ClockManager::createInfiniteTimer(uint64_t delay, std::function<void()> consumer) {
		uint64_t startedAt = this->now();

		int id = ID++;
		clocks[id] = new RepeatingTimer(id, startedAt, consumer, delay);
	}
	bool ClockManager::update(bool force) {
		uint64_t now = this->now();

		std::vector<int> clocksToRemove;
		for (auto& clock : clocks)
			if (clock.second->update(now, force))
				clocksToRemove.push_back(clock.first);

		for (int i : clocksToRemove) {
			delete clocks[i];
			clocks.erase(i);
		}

		return !clocks.empty();
	}
	uint64_t ClockManager::now() {
		using namespace std::chrono;
		return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
	}
}