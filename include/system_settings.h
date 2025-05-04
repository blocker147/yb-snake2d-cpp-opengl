#ifndef SNAKE2D_SYSTEM_SETTINGS_H
#define SNAKE2D_SYSTEM_SETTINGS_H

namespace Snake2d {
	class SystemSettings {
	public:
		SystemSettings(int screenWidth, int screenHeight): screenWidth(screenWidth), screenHeight(screenHeight) {}

		int screenWidth, screenHeight;
	};
}

#endif