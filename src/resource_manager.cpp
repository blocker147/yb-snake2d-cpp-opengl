#include "resource_manager.h"

namespace Snake2d {
	ShaderManager::ShaderManager(int width, int height) {
		createCellShader(width, height);
		createMenuShader();
		createParticlesShader();
	}
	void ShaderManager::createCellShader(int width, int height) {
		std::string vertexShader = Snake2d::SHADER_DIR + "shader_field.vs";
		std::string fragmentShader = Snake2d::SHADER_DIR + "shader_field.fs";
		Shader* shader = new Shader(vertexShader.c_str(), fragmentShader.c_str());

		const int VAO_INDEX_ID = 0;
		const int VAO_INDEX_SIZE = 1;

		const int VAO_POS_ID = 1;
		const int VAO_POS_SIZE = 18;

		const int VAO_COLOR_ID = 2;
		const int VAO_COLOR_SIZE = 3;

		const int VAO_TEXCOORD_ID = 3;
		const int VAO_TEXCOORD_SIZE = 2;

		struct VAO_DATA {
			float index;
			float vertices[3];
			float color[3];
			float textureCoords[2];
		};

		float openglNormalizedCellWidth = 2.0f / width;
		float openglNormalizedCellHeight = 2.0f / height;
		std::vector<VAO_DATA> vertices;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				float left_x = -1.0f + x * openglNormalizedCellWidth;
				float right_x = -1.0f + (x + 1) * openglNormalizedCellWidth;
				float top_y = 1.0f - y * openglNormalizedCellHeight;
				float bottom_y = 1.0f - (y + 1) * openglNormalizedCellHeight;

				int index = width * (y + 1) - width + x;

				float positions[6][3] = {
					{ left_x,  top_y,    0.0f },
					{ left_x,  bottom_y, 0.0f },
					{ right_x, bottom_y, 0.0f },

					{ right_x, bottom_y, 0.0f },
					{ right_x, top_y,    0.0f },
					{ left_x,  top_y,    0.0f }
				};

				float texCoords[6][2] = {
					{ 0.0f, 1.0f }, // top-left
					{ 0.0f, 0.0f }, // bottom-left
					{ 1.0f, 0.0f }, // bottom-right

					{ 1.0f, 0.0f }, // bottom-right
					{ 1.0f, 1.0f }, // top-right
					{ 0.0f, 1.0f }  // top-left
				};

				bool isEven = ((x + y) % 2 == 0);
				// Set data for '6' vertices (Square)
				for (int i = 0; i < 6; i++) {
					VAO_DATA v;

					// VAO_INDEX
					v.index = index;

					// VAO_VERTICES_COORDINATES
					v.vertices[0] = positions[i][0];
					v.vertices[1] = positions[i][1];
					v.vertices[2] = positions[i][2];

					// VAO_COLOR
					v.color[0] = isEven ? 0.184f : 0.224f;
					v.color[1] = isEven ? 0.184f : 0.224f;
					v.color[2] = isEven ? 0.184f : 0.224f;

					// VAO_TEXTURE_COORDINATES
					v.textureCoords[0] = texCoords[i][0];
					v.textureCoords[1] = texCoords[i][1];

					vertices.push_back(v);
				}
			}
		}

		unsigned int VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VAO_DATA) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		GLsizei stride = sizeof(VAO_DATA);

		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 1));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 4));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 7));
		glEnableVertexAttribArray(3);

		shaders[ShaderType::CELL] = ShaderData(shader, VAO);
	}
	void ShaderManager::createMenuShader() {
		std::string vertexShader = Snake2d::SHADER_DIR + "shader_menu.vs";
		std::string fragmentShader = Snake2d::SHADER_DIR + "shader_menu.fs";
		Shader* menuShader = new Shader(vertexShader.c_str(), fragmentShader.c_str());

		float vertices[] = {
			-0.5, 0.5, 0.0,
			0.5, 0.5, 0.0,
			-0.5, -0.5, 0.0,

			-0.5, -0.5, 0.0,
			0.5, -0.5, 0.0,
			0.5, 0.5, 0.0
		};

		unsigned int VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		shaders[ShaderType::MENU] = ShaderData(menuShader, VAO);
	}
	void ShaderManager::createParticlesShader() {
		std::string vertexShader = Snake2d::SHADER_DIR + "shader_particles.vs";
		std::string fragmentShader = Snake2d::SHADER_DIR + "shader_particles.fs";

		Shader* shader = new Shader(vertexShader.c_str(), fragmentShader.c_str());

		float quad[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,

			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f
		};

		unsigned int quadVBO, quadVAO;
		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);

		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);


		int maxParticles = 500; // TODO: how much???
		unsigned int particleVBO;
		glGenBuffers(1, &particleVBO);
		glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
		glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);

		// Position
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, offset));
		glVertexAttribDivisor(1, 1);

		// Color
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
		glVertexAttribDivisor(2, 1);

		// Size
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));
		glVertexAttribDivisor(3, 1);

		shaders[ShaderType::PARTICLES] = ShaderData(shader, quadVAO, particleVBO);
	}

	TextureManager::TextureManager() {
		std::string texturesPath = Snake2d::TEXTURE_DIR;
		loadTexture(TextureType::APPLE, (texturesPath + "apple.png").c_str());
		loadTexture(TextureType::SNAKE_HEAD, (texturesPath + "snake-head.png").c_str());
		loadTexture(TextureType::SNAKE_TAIL, (texturesPath + "snake-tail.png").c_str());
		loadTexture(TextureType::SNAKE_BODY_STRAIGHT, (texturesPath + "snake-body-straight.png").c_str());
		loadTexture(TextureType::SNAKE_BODY_ROTATED, (texturesPath + "snake-body-rotated.png").c_str());
		loadTexture(TextureType::SNAKE_CORPSE, (texturesPath + "snake-corpse.png").c_str());
		loadTexture(TextureType::SNAKE_BONE, (texturesPath + "snake-bone.png").c_str());
		loadTexture(TextureType::APPLE_LINE_SPAWNER, (texturesPath + "apple-line-spawner.png").c_str());
		loadTexture(TextureType::SNAKE_BONE_DESTROYER, (texturesPath + "snake-bone-destroyer.png").c_str());
	}
	void TextureManager::loadTexture(TextureType type, const char* filename, bool flipVertically) {
		unsigned int texture;
		glGenTextures(1, &texture);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(flipVertically);
		unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
		if (data) {
			GLenum format{};
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			std::cout << "Failed to load texture: " << filename << '\n';
		}
		stbi_image_free(data);

		textures[type] = texture;
	}

	AudioManager::AudioManager() {
		soundEngine = irrklang::createIrrKlangDevice();

		std::string audioPath = Snake2d::AUDIO_DIR;
		audios[AudioType::ANY_MENU_BACKGROUND_MUSIC]	= audioPath + "background.wav";
		audios[AudioType::APPLE_EATEN_RANDOM]			= audioPath + "apple_eaten_0.wav";
		audios[AudioType::APPLE_EATEN_0]				= audioPath + "apple_eaten_0.wav";
		audios[AudioType::APPLE_EATEN_1]				= audioPath + "apple_eaten_1.wav";
		audios[AudioType::APPLE_EATEN_2]				= audioPath + "apple_eaten_2.wav";
		audios[AudioType::CORPSE_EATEN_RANDOM]			= audioPath + "corpse_eaten_0.wav";
		audios[AudioType::CORPSE_EATEN_0]				= audioPath + "corpse_eaten_0.wav";
		audios[AudioType::CORPSE_EATEN_1]				= audioPath + "corpse_eaten_1.wav";
		audios[AudioType::CORPSE_EATEN_2]				= audioPath + "corpse_eaten_2.wav";
		audios[AudioType::BONE_DESTROYED_RANDOM]		= audioPath + "bone_destroyed_0.wav";
		audios[AudioType::BONE_DESTROYED_0]				= audioPath + "bone_destroyed_0.wav";
		audios[AudioType::BONE_DESTROYED_1]				= audioPath + "bone_destroyed_1.wav";
	}
	void AudioManager::play(AudioType type, bool playLooped, bool isBackground) {
		if (isBackground) {
			irrklang::ISound* sound = soundEngine->play2D(audios[type].c_str(), playLooped, false, true);
			if (sound) {
				sound->setVolume(0.3f);
				sound->drop();
			}
		}
		else
			soundEngine->play2D(audios[type].c_str(), playLooped);
	}
	void AudioManager::playRandomly(AudioType type, bool playLooped) {
		std::vector<AudioType> randomAudios;
		switch (type) {
			case AudioType::APPLE_EATEN_RANDOM: {
				randomAudios.push_back(AudioType::APPLE_EATEN_0);
				randomAudios.push_back(AudioType::APPLE_EATEN_1);
				randomAudios.push_back(AudioType::APPLE_EATEN_2);
				break;
			}
			case AudioType::CORPSE_EATEN_RANDOM: {
				randomAudios.push_back(AudioType::CORPSE_EATEN_0);
				randomAudios.push_back(AudioType::CORPSE_EATEN_1);
				randomAudios.push_back(AudioType::CORPSE_EATEN_2);
				break;
			}
			case AudioType::BONE_DESTROYED_RANDOM: {
				randomAudios.push_back(AudioType::BONE_DESTROYED_0);
				randomAudios.push_back(AudioType::BONE_DESTROYED_1);
				break;
			}
			default:
				std::cout << "WARNING::Wrong type provided which doesn't support random audio: " << type << std::endl;
				break;
		}

		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, randomAudios.size() - 1);

		int randomIndex = dis(gen);
		AudioType chosenAudio = randomAudios[randomIndex];

		soundEngine->play2D(audios[chosenAudio].c_str(), playLooped);
	}

	void SettingsManager::readFromFile() {
		std::string settingsPath = Snake2d::SETTINGS_DIR;

		std::ifstream inf{ (settingsPath + "app_settings.txt").c_str() };

		if (!inf) {
			std::cout << "WARNING::app_settings.txt file not found at: " << settingsPath + "app_settings.txt" << std::endl;
			std::cout << "INFO::Setting default values for screen height & width" << std::endl;
			screenWidth = 800;
			screenHeight = 800;
			return;
		}

		std::string line{};
		while (std::getline(inf, line)) {

			std::size_t equalAt = line.find("=");
			if (equalAt != std::string::npos) {
				std::string key = line.substr(0, equalAt);
				std::string value = line.substr(equalAt + 1);

				if (key == "screen_width") {
					screenWidth = std::stoi(value);
					std::cout << "INFO::Setting screen_width to: " << screenWidth << std::endl;
				}
				else if (key == "screen_height") {
					screenHeight = std::stoi(value);
					std::cout << "INFO::Setting screen_height to: " << screenHeight << std::endl;
				}
			}
		}
	}
	
	void ParticleManager::removeDead() {
		for (auto& [type, particles] : particlesMap)
			particles.erase(
				std::remove_if(particles.begin(), particles.end(),
					[](Particle& p) {
						return p.life <= 0.0f;
					}),
				particles.end()
			);
	}
	void ParticleManager::add(ParticleType type, std::pair<float, float> position) {
		auto [clipX, clipY] = position;
		constexpr float M_PI = 3.14159265358979323846f;

		int count = 0;
		for (auto& [_, particles] : particlesMap)
			count += particles.size();

		if (count >= 450) {
			std::cout << "WARNING::Exceeded particles limit (450)" << std::endl;
			return;
		}

		switch (type) {
			case ParticleType::APPLE_EATEN: {
				int eatenAppleParticles = 5; 
				for (int i = 0; i < eatenAppleParticles; i++) {
					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_real_distribution<float> dist(-0.05f, 0.05f);

					float randomOffsetX = dist(gen);
					float randomOffsetY = dist(gen);

					clipX += randomOffsetX;
					clipY += randomOffsetY;

					float life = 2.0f + static_cast<float>(rand()) / RAND_MAX * 0.2f;
					float maxLife = life;
					std::uniform_real_distribution<float> speedDist(0.1f, 0.5f);
					std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);

					float angle = angleDist(gen);
					float speed = speedDist(gen);
					float size = 0.01f;
					glm::vec2 offset = glm::vec2(clipX, clipY);
					glm::vec4 color = glm::vec4(1.0f, 0.1f + dist(gen) * 0.2f, 0.1f, 1.0f);
					glm::vec2 velocity = glm::vec2(cos(angle), sin(angle)) * speed;

					Snake2d::Particle particle = Snake2d::Particle{
						offset, color, velocity, size, life, maxLife
					};
					particlesMap[ParticleType::APPLE_EATEN].push_back(particle);

				}
				break;
			}
			case ParticleType::CORPSE_EATEN: {
				int eatenCorpseParticles = 5;
				for (int i = 0; i < eatenCorpseParticles; i++) {
					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_real_distribution<float> dist(-0.05f, 0.05f);

					float randomOffsetX = dist(gen);
					float randomOffsetY = dist(gen);

					clipX += randomOffsetX;
					clipY += randomOffsetY;

					float life = 2.0f + static_cast<float>(rand()) / RAND_MAX * 0.2f;
					float maxLife = life;
					std::uniform_real_distribution<float> speedDist(0.1f, 0.5f);
					std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);

					float angle = angleDist(gen);
					float speed = speedDist(gen);
					float size = 0.01f;
					glm::vec2 offset = glm::vec2(clipX, clipY);
					glm::vec4 color = glm::vec4(
						0.5f + dist(gen) * 0.1f,
						0.9f + dist(gen) * 0.05f,
						0.2f + dist(gen) * 0.05f,
						1.0f
					);
					glm::vec2 velocity = glm::vec2(cos(angle), sin(angle)) * speed;

					Snake2d::Particle particle = Snake2d::Particle{
						offset, color, velocity, size, life, maxLife
					};
					particlesMap[ParticleType::CORPSE_EATEN].push_back(particle);

				}
				break;
			}
			case ParticleType::BONE_DESTROYED: {
				int boneDestroyedParticles = 5;
				for (int i = 0; i < boneDestroyedParticles; i++) {
					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_real_distribution<float> dist(-0.1f, 0.1f);

					float randomOffsetX = dist(gen);
					float randomOffsetY = dist(gen);

					clipX += randomOffsetX;
					clipY += randomOffsetY;

					float life = 2.0f + static_cast<float>(rand()) / RAND_MAX * 0.2f;
					float maxLife = life;
					std::uniform_real_distribution<float> speedDist(0.1f, 0.5f);
					std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);

					float angle = angleDist(gen);
					float speed = speedDist(gen);
					float size = 0.01f;
					glm::vec2 offset = glm::vec2(clipX, clipY);
					glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.9f + dist(gen) * 0.1f, 1.0f);
					glm::vec2 velocity = glm::vec2(cos(angle), sin(angle)) * speed;

					Snake2d::Particle particle = Snake2d::Particle{
						offset, color, velocity, size, life, maxLife
					};
					particlesMap[ParticleType::BONE_DESTROYED].push_back(particle);

				}
				break;
			}
		}
	}
	void ParticleManager::update(float deltaTime) {
		for (auto& [type, particles] : particlesMap) {
			switch (type) {
				case ParticleType::APPLE_EATEN: {
					for (Particle& p : particles) {
						p.life -= deltaTime;
						if (p.life > 0.0f) {
							p.offset += p.velocity * deltaTime;

							float lifeRatio = p.life / p.maxLife;
							p.color.a = lifeRatio;
							p.size *= lifeRatio;
						}
					}
					break;
				}
				case ParticleType::CORPSE_EATEN: {
					for (Particle& p : particles) {
						p.life -= deltaTime;
						if (p.life > 0.0f) {
							p.offset += p.velocity * deltaTime;

							float lifeRatio = p.life / p.maxLife;
							p.color.a = lifeRatio;
							p.size *= lifeRatio;
						}
					}
					break;
				}
				case ParticleType::BONE_DESTROYED: {
					for (Particle& p : particles) {
						p.life -= deltaTime;
						if (p.life > 0.0f) {
							p.offset += p.velocity * deltaTime;

							float lifeRatio = p.life / p.maxLife;
							p.color.a = lifeRatio;
							p.size *= lifeRatio;
						}
					}
					break;
				}
			}
		}
	}
	std::vector<Particle> ParticleManager::getAll() {
		std::vector<Particle> allParticles;
		for (auto& [_, particles] : particlesMap)
			for (Particle& particle : particles)
				allParticles.push_back(particle);

		return allParticles;
	}
}