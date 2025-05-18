#include "file_utils.h"

namespace Snake2d {
#ifdef _DEBUG
	const std::filesystem::path BASE_PATH = std::filesystem::current_path().parent_path().parent_path().parent_path();
	const std::string prefix = "../../../";
#else
	const std::string prefix = "";
	const std::filesystem::path BASE_PATH = std::filesystem::current_path();
#endif
	// TODO: not the best solution
	const std::string FONT_DIR = (BASE_PATH / "resources" / "fonts/").string();
	const std::string SHADER_DIR = prefix + "resources/shaders/";
	const std::string TEXTURE_DIR = prefix + "resources/textures/";
	const std::string AUDIO_DIR = prefix + "resources/audio/";
	const std::string SETTINGS_DIR = prefix + "resources/settings/";
}