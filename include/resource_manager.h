#ifndef SNAKE2D_RESOURCE_MANAGER_H
#define SNAKE2D_RESOURCE_MANAGER_H

#include <stb_image.h>

#include <map>
#include <iostream>

#include "shader.h"
#include "file_utils.h"
#include <irrklang/irrKlang.h>

namespace Snake2d {
	enum ShaderType { CELL, MENU };
	class ShaderManager {
	private:
		std::map<ShaderType, std::pair<Shader*, unsigned int>> shaders;

		void createCellShader(int width, int height);
		void createMenuShader();
	public:
		ShaderManager(int width, int height);

		std::pair<Shader*, unsigned int> getShaderAndVAO(ShaderType type) {	return shaders[type]; }
	};

	enum TextureType {
		APPLE,
		SNAKE_HEAD,
		SNAKE_BODY_STRAIGHT,
		SNAKE_BODY_ROTATED,
		SNAKE_TAIL,
		SNAKE_CORPSE,
		SNAKE_BONE,
		APPLE_LINE_SPAWNER,
		SNAKE_BONE_DESTROYER
	};
	class TextureManager {
	private:
		std::map<TextureType, unsigned int> textures;

		void loadTexture(TextureType type, const char* filename, bool flipVertically = true);
	public:
		TextureManager();

		unsigned int getTextureId(TextureType type) { return textures[type]; }
	};

	enum AudioType { ANY_MENU_BACKGROUND_MUSIC };
	class AudioManager {
	private:
		irrklang::ISoundEngine* soundEngine;
		std::map<AudioType, std::string> audios;
	public:
		AudioManager();
		~AudioManager() {
			delete soundEngine;
		}

		void play(AudioType type, bool playLooped = false);
	};

	class SettingsManager {
	private:
		int screenWidth, screenHeight;

		void readFromFile();
	public:
		SettingsManager() { readFromFile(); }

		int getScreenWidth() const { return screenWidth; }
		int getScreenHeight() const { return screenHeight; }
	};
}

#endif