#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <filesystem>

#include "shader.h"

#include "game_object.h"
#include "user_input.h"
#include "world.h"
#include "resource_manager.h"
#include "game_state.h"
#include "font_renderer.h"

Snake2d::GameState gameState;
const int WORLD_WIDTH = 20, WORLD_HEIGHT = 20;

GLFWwindow* createWindow(Snake2d::SettingsManager&);
void framebufferSizeCallback(GLFWwindow*, int, int);
void cursorPosCallback(GLFWwindow*, double, double);
void mouseButtonCallback(GLFWwindow*, int, int, int);
void keyCallback(GLFWwindow*, int, int, int, int);
bool mouseOnRectangle = false;

void handleMainMenu(Snake2d::GameState&, Snake2d::FontRenderer&, Snake2d::SettingsManager&, Snake2d::ShaderManager*);
void handlePlayingGameMenu(Snake2d::GameState&, Snake2d::FontRenderer&, Snake2d::SettingsManager&, Snake2d::ShaderManager*, Snake2d::TextureManager&);
void handleGameOverMenu(Snake2d::GameState&, Snake2d::FontRenderer&, Snake2d::SettingsManager&, Snake2d::ShaderManager*);

int main()
{
	std::cout << std::filesystem::current_path() << std::endl;
	Snake2d::SettingsManager settingsManager;

	GLFWwindow* window = createWindow(settingsManager);
	if (window == NULL) return -1;

	// UI_INITIALIZATION
	// TODO: Move FontRenderer to resource_manager.h and rename it to FontManager
	Snake2d::FontRenderer fontRenderer(settingsManager);
	fontRenderer.loadFont(Snake2d::FontType::ANTONIO_BOLD, "Antonio-Bold.ttf");
	Snake2d::ShaderManager* shaderManager = new Snake2d::ShaderManager(WORLD_WIDTH, WORLD_HEIGHT);
	Snake2d::TextureManager textureManager;
	Snake2d::AudioManager audioManager;

	audioManager.play(Snake2d::AudioType::ANY_MENU_BACKGROUND_MUSIC, true);
	
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		switch (gameState.getView()) {
			case Snake2d::GameView::MAIN_MENU:
				handleMainMenu(gameState, fontRenderer, settingsManager, shaderManager); break;
			case Snake2d::GameView::PLAYING_GAME_MENU:
				handlePlayingGameMenu(gameState, fontRenderer, settingsManager, shaderManager, textureManager); break;
			case Snake2d::GameView::GAME_OVER_MENU:
				handleGameOverMenu(gameState, fontRenderer, settingsManager, shaderManager); break;
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	gameState.cleanUp();
	delete shaderManager;
	shaderManager = nullptr;

	return 0;
}

GLFWwindow* createWindow(Snake2d::SettingsManager& settingsManager)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(settingsManager.getScreenWidth(), settingsManager.getScreenHeight(), "Snake2d", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << '\n';
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load GLAD" << '\n';
	}
	glViewport(0, 0, settingsManager.getScreenWidth(), settingsManager.getScreenHeight());

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetKeyCallback(window, keyCallback);

	return window;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void cursorPosCallback(GLFWwindow* window, double x, double y)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	float ndcX = (float)x / width * 2.0f - 1.0f;
	float ndcY = 1.0f - (float)y / height * 2.0f;

	float top = 0.5;
	float left = -0.5;
	float right = 0.5;
	float bottom = -0.5;

	if (ndcX >= left && ndcX <= right && ndcY >= bottom && ndcY <= top)
		mouseOnRectangle = true;
	else
		mouseOnRectangle = false;
}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mouseOnRectangle) {
		if (gameState.getView() == Snake2d::GameView::MAIN_MENU)
			gameState.changeView(Snake2d::GameView::PLAYING_GAME_MENU);
		else if (gameState.getView() == Snake2d::GameView::GAME_OVER_MENU)
			gameState.changeView(Snake2d::GameView::MAIN_MENU);
	}
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		if (gameState.getView() == Snake2d::GameView::MAIN_MENU)
			glfwSetWindowShouldClose(window, GL_TRUE);
		else if (gameState.getView() == Snake2d::GameView::PLAYING_GAME_MENU)
			gameState.changeView(Snake2d::GameView::GAME_OVER_MENU);
		else if (gameState.getView() == Snake2d::GameView::GAME_OVER_MENU)
			gameState.changeView(Snake2d::GameView::MAIN_MENU);
	}
	if (gameState.getView() == Snake2d::GameView::PLAYING_GAME_MENU) {
		if ((key == GLFW_KEY_W || key == GLFW_KEY_UP) && action == GLFW_PRESS)
			UserInput::updateDirection(UserInput::Direction::UP);
		if ((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && action == GLFW_PRESS)
			UserInput::updateDirection(UserInput::Direction::DOWN);
		if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && action == GLFW_PRESS)
			UserInput::updateDirection(UserInput::Direction::LEFT);
		if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS)
			UserInput::updateDirection(UserInput::Direction::RIGHT);
	}
}

void handleMainMenu(Snake2d::GameState& gameState, Snake2d::FontRenderer& fontRenderer, Snake2d::SettingsManager& settingsManager, Snake2d::ShaderManager* shaderManager)
{
	// GAME_LOGIC_SETUP
	gameState.initialize();

	// UI_RENDERING
	auto [shader, VAO] = shaderManager->getShaderAndVAO(Snake2d::ShaderType::MENU);
	shader->use();
	shader->setBool("hover", mouseOnRectangle);
	shader->setBool("isOver", false);
	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// FONT_RENDERING
	fontRenderer.renderText(
		Snake2d::FontType::ANTONIO_BOLD,
		"Play",
		settingsManager.getScreenWidth() / 2 - 30,
		settingsManager.getScreenHeight() / 2,
		1.0f,
		glm::vec3(0.0f, 0.0f, 0.0f)
	);

	fontRenderer.renderText(
		Snake2d::FontType::ANTONIO_BOLD,
		"Control: WSAD or Arrows. Escape exits from PlayingGameMenu or from GameOverMenu",
		0.0f,
		settingsManager.getScreenHeight() - 50.0f,
		0.5f,
		glm::vec3(0.0f, 0.0f, 0.0f)
	);
}
void handlePlayingGameMenu(Snake2d::GameState& gameState, Snake2d::FontRenderer& fontRenderer, Snake2d::SettingsManager& settingsManager, Snake2d::ShaderManager* shaderManager, Snake2d::TextureManager& textureManager)
{
	mouseOnRectangle = false;

	// GAME_LOGIC_UPDATE
	gameState.update(UserInput::direction);

	// UI_RENDERING
	auto world = gameState.getWorld();
	float* openglWorld		= new float[WORLD_WIDTH * WORLD_HEIGHT];
	float* openglRotations	= new float[WORLD_WIDTH * WORLD_HEIGHT];
	for (int x = 0; x < world.size(); x++)
		for (int y = 0; y < world[x].size(); y++) {
			Snake2d::GameObject* go = world[y][x];
			int index = y * WORLD_WIDTH + x;
			openglRotations[index] = go->getRotation();
			switch (go->getType()) {
				case Snake2d::GameObject::Type::NONE:					openglWorld[index] = 0; break;
				case Snake2d::GameObject::Type::SNAKE_CORPSE:			openglWorld[index] = 1; break;
				case Snake2d::GameObject::Type::SNAKE_HEAD:				openglWorld[index] = 2; break;
				case Snake2d::GameObject::Type::SNAKE_BODY: {
					Snake2d::SnakeBody* body = dynamic_cast<Snake2d::SnakeBody*>(go);
					if (body->segment == Snake2d::SnakeBody::Segment::Straight)
						openglWorld[index] = 3;
					else if (body->segment == Snake2d::SnakeBody::Segment::Rotated)
						openglWorld[index] = 8;
					else
						std::cout << "WARNING: unexpected SnakeBody Segment provided: " << body->segment << std::endl;
					break;
				}
				case Snake2d::GameObject::Type::APPLE:					openglWorld[index] = 4;	break;
				case Snake2d::GameObject::Type::APPLE_LINE_SPAWNER:		openglWorld[index] = 5; break;
				case Snake2d::GameObject::Type::SNAKE_BONE:				openglWorld[index] = 6; break;
				case Snake2d::GameObject::Type::SNAKE_TAIL:				openglWorld[index] = 7; break;
				case Snake2d::GameObject::Type::SNAKE_BONE_DESTROYER:	openglWorld[index] = 9; break;
			}
		}

	auto [shader, VAO] = shaderManager->getShaderAndVAO(Snake2d::ShaderType::CELL);
	shader->use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager.getTextureId(Snake2d::TextureType::APPLE));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureManager.getTextureId(Snake2d::TextureType::SNAKE_HEAD));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureManager.getTextureId(Snake2d::TextureType::SNAKE_TAIL));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureManager.getTextureId(Snake2d::TextureType::SNAKE_BODY_STRAIGHT));
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureManager.getTextureId(Snake2d::TextureType::SNAKE_BODY_ROTATED));
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, textureManager.getTextureId(Snake2d::TextureType::SNAKE_CORPSE));
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, textureManager.getTextureId(Snake2d::TextureType::SNAKE_BONE));
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, textureManager.getTextureId(Snake2d::TextureType::APPLE_LINE_SPAWNER));
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, textureManager.getTextureId(Snake2d::TextureType::SNAKE_BONE_DESTROYER));

	shader->setInt("u_texture_apple",					0);
	shader->setInt("u_texture_snake_head",				1);
	shader->setInt("u_texture_snake_body_tail",			2);
	shader->setInt("u_texture_snake_body_straight",		3);
	shader->setInt("u_texture_snake_body_rotated",		4);
	shader->setInt("u_texture_snake_corpse",			5);
	shader->setInt("u_texture_snake_bone",				6);
	shader->setInt("u_texture_apple_line_spawner",		7);
	shader->setInt("u_texture_snake_bone_destroyer",	8);

	shader->setFloatArray("u_world_size",		WORLD_WIDTH * WORLD_HEIGHT, openglWorld);
	shader->setFloatArray("u_world_rotations",	WORLD_WIDTH * WORLD_HEIGHT, openglRotations);
	delete[] openglWorld;
	delete[] openglRotations;

	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, WORLD_WIDTH * WORLD_HEIGHT * 6);
	glDisable(GL_BLEND);

	// FONT_RENDERING
	fontRenderer.renderText(
		Snake2d::FontType::ANTONIO_BOLD,
		"Score: " + std::to_string(gameState.getScore()) + " Life: " + std::to_string(gameState.getLife()),
		0.0f,
		settingsManager.getScreenHeight() - 50.0f,
		0.5f,
		glm::vec3(0.0f, 0.0f, 0.0f)
	);
}
void handleGameOverMenu(Snake2d::GameState& gameState, Snake2d::FontRenderer& fontRenderer, Snake2d::SettingsManager& settingsManager, Snake2d::ShaderManager* shaderManager)
{
	// GAME_LOGIC_CLEANUP
	gameState.cleanUp();

	// UI_RENDERING
	auto [shader, VAO] = shaderManager->getShaderAndVAO(Snake2d::ShaderType::MENU);
	shader->use();
	shader->setBool("hover", mouseOnRectangle);
	shader->setBool("isOver", true);
	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// FONT_RENDERING
	fontRenderer.renderText(
		Snake2d::FontType::ANTONIO_BOLD,
		"Score: " + std::to_string(gameState.getScore()) + " Life: " + std::to_string(gameState.getLife()),
		0.0f,
		settingsManager.getScreenHeight() - 50.0f,
		0.5f,
		glm::vec3(0.0f, 0.0f, 0.0f)
	);
	fontRenderer.renderText(
		Snake2d::FontType::ANTONIO_BOLD,
		"Exit",
		settingsManager.getScreenWidth() / 2 - 30,
		settingsManager.getScreenHeight() / 2,
		1.0f,
		glm::vec3(0.0f, 0.0f, 0.0f)
	);
}
