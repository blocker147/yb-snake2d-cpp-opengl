#ifndef SNAKE2D_FONT_RENDERER_H
#define SNAKE2D_FONT_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>

#include "shader.h"
#include "file_utils.h"

namespace Snake2d {
	enum FontType { ANTONIO_BOLD };

	class FontRenderer {
	private:
		struct Character {
			unsigned int TextureID;
			glm::ivec2   Size;
			glm::ivec2   Bearing;
			unsigned int Advance;
		};
		std::map<FontType, std::map<char, Character>> Characters;

		unsigned int TEXT_VAO, TEXT_VBO;
		Shader* shader;

		void initializeVAO() {
			glGenVertexArrays(1, &TEXT_VAO);
			glGenBuffers(1, &TEXT_VBO);

			glBindVertexArray(TEXT_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, TEXT_VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
	public:
		FontRenderer(SettingsManager& settingsManager) {
			float screenWidth = settingsManager.getScreenWidth();
			float screenHeight = settingsManager.getScreenHeight();

			std::string vertexShader = Snake2d::SHADER_DIR + "shader_text.vs";
			std::string fragmentShader = Snake2d::SHADER_DIR + "shader_text.fs";
			shader = new Shader(vertexShader.c_str(), fragmentShader.c_str());

			initializeVAO();

			shader->use();
			glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f, static_cast<float>(screenHeight));
			glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		}
		~FontRenderer() = default;

		int loadFont(FontType fontType, std::string fontName) {
			FT_Library ft;
			if (FT_Init_FreeType(&ft)) {
				std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
				return -1;
			}

			std::string font_name = Snake2d::FONT_DIR + fontName;
			std::cout << font_name << std::endl;

			if (font_name.empty()) {
				std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
				return -1;
			}

			FT_Face face;
			if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
				std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
				return -1;
			}
			else {
				FT_Set_Pixel_Sizes(face, 0, 48);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				std::map<char, Character> charMetadata;
				for (unsigned char c = 0; c < 128; c++) {
					if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
						std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
						continue;
					}

					unsigned int texture;
					glGenTextures(1, &texture);
					glBindTexture(GL_TEXTURE_2D, texture);
					glTexImage2D(
						GL_TEXTURE_2D,
						0,
						GL_RED,
						face->glyph->bitmap.width,
						face->glyph->bitmap.rows,
						0,
						GL_RED,
						GL_UNSIGNED_BYTE,
						face->glyph->bitmap.buffer
					);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					Character character = {
						texture,
						glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
						glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
						static_cast<unsigned int>(face->glyph->advance.x)
					};
					charMetadata.insert(std::pair<char, Character>(c, character));
				}
				Characters.insert(std::pair<FontType, std::map<char, Character>>(fontType, charMetadata));
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			FT_Done_Face(face);
			FT_Done_FreeType(ft);

			return 0;
		}

		void renderText(FontType fontType, std::string text, float x, float y, float scale, glm::vec3 color)
		{
			shader->use();
			glUniform3f(glGetUniformLocation(shader->ID, "textColor"), color.x, color.y, color.z);
			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(TEXT_VAO);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			std::string::const_iterator c;
			for (c = text.begin(); c != text.end(); c++)
			{
				Character ch = Characters[fontType][*c];

				float xpos = x + ch.Bearing.x * scale;
				float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

				float w = ch.Size.x * scale;
				float h = ch.Size.y * scale;
				float vertices[6][4] = {
					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos,     ypos,       0.0f, 1.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },

					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },
					{ xpos + w, ypos + h,   1.0f, 0.0f }
				};
				glBindTexture(GL_TEXTURE_2D, ch.TextureID);
				glBindBuffer(GL_ARRAY_BUFFER, TEXT_VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				x += (ch.Advance >> 6) * scale;
			}
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glDisable(GL_BLEND);
		}
	};
}

#endif