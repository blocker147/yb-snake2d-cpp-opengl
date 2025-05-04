#ifndef SNAKE_2D_TIMER
#define SNAKE_2D_TIMER

#include <chrono>
#include <map>
#include <vector>
#include <functional>

namespace Snake2d {
	class Clock{
	private:
		uint64_t id;
		uint64_t startedAt;
		std::function<void()> consumer;
	public:
		Clock(uint64_t id, uint64_t startedAt, std::function<void()> consumer);
		uint64_t getId() const;
		void consume() const;
		virtual bool update(const uint64_t& now, bool force = false) = 0;
	
		static uint64_t whatTimeIsIt() {
			using namespace std::chrono;
			return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
		}
	};

	class RepeatingTimer : public Clock {
	private:
		uint64_t delay;
		int times;
		uint64_t updatedAt;
		bool isInfinite;
	public:
		RepeatingTimer(uint64_t id, uint64_t startedAt, std::function<void()> consumer, uint64_t delay);
		bool update(const uint64_t& now, bool force = false);
		bool shouldUpdate(const uint64_t& now, bool force = false);
	};

	class ClockManager {
	private:
		std::map<int, Clock*> clocks;
		uint64_t ID = 0;
	public:
		~ClockManager();
		void createInfiniteTimer(uint64_t delay, std::function<void()> consumer);
		bool update(bool force = false);
		uint64_t now();
	};
}

#endif