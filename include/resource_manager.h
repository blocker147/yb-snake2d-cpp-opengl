#ifndef SNAKE2D_RESOURCE_MANAGER_H
#define SNAKE2D_RESOURCE_MANAGER_H

#include <stb_image.h>

#include <map>
#include <iostream>
#include <random>

#include "shader.h"
#include "file_utils.h"
#include <irrklang/irrKlang.h>

namespace Snake2d {
	struct ShaderData {
		Shader* shader = nullptr;
		GLuint vao = 0;
		GLuint vbo = 0;
	};
	enum ShaderType { CELL, MENU, PARTICLES };
	class ShaderManager {
	private:
		std::map<ShaderType, ShaderData> shaders;

		void createCellShader(int width, int height);
		void createMenuShader();
		void createParticlesShader();
	public:
		ShaderManager(int width, int height);
		ShaderData& get(ShaderType type) { return shaders.at(type); }
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

	enum ParticleType { APPLE_EATEN };
	struct Particle {				// Represented as a square
		glm::vec2 offset;			// offset location relative to initial position - middle of the screen
		glm::vec4 color;			// rgba color
		glm::vec2 velocity;			// in which direction to move
		float size;					// size of square
		float life;					// current life time
		float maxLife;				// initial life time
	};
	class ParticleManager {
	private:
		std::map<ParticleType, std::vector<Particle>> particlesMap;
	public:
		void removeDead();
		void add(ParticleType type, std::pair<float, float> position);
		void update(float deltaTime);
		std::vector<Particle> getAll();
	};
}

#endif