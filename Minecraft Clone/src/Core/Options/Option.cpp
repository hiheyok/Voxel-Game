#include "Option.h"

Options::Options() {
	SetOptionNameTable();

	//Set values from option file
	if (!FileManager::CheckFile("options.txt")) {
		GenerateOptionFile();
		return; //check if option file exist
	}

	File file = FileManager::GetFile("options.txt");

	std::vector<std::string> tokens = file.GetToken(':');

	ProcessTokens(tokens);
}

bool Options::SetValue(std::string name, std::string value) {
	int val = stoi(value);

	if (!option_name_.count(name)) {
		g_logger.LogError("Option", "Unknown option: " + name);
		return false;
	}

	*option_name_[name] = val;
	return true;
}

void Options::ProcessTokens(std::vector<std::string> tokens) {
	bool success = true;

	for (int i = 0; i < (tokens.size() / 2); i++) {
		std::string name = tokens[2 * i];
		std::string val = tokens[2 * i + 1];
		g_logger.LogDebug("Options", name + ":" + val);
		if (!SetValue(name, val)) {
			success = false;
		}
	}
	//Regenerate file if it has an error in it
	if (!success) {
		FileManager::DeleteFile("options.txt");
		GenerateOptionFile();
	}
}

void Options::SetOptionNameTable() {
	option_name_.insert(std::pair<std::string, int*>("HorizontalRenderDistance", &horizontal_render_distance_));
	option_name_.insert(std::pair<std::string, int*>("VerticalRenderDistance", &vertical_render_distance_));
	option_name_.insert(std::pair<std::string, int*>("WorldGenThreads", &world_gen_threads_));
	option_name_.insert(std::pair<std::string, int*>("MeshThreads", &mesh_threads_));
	option_name_.insert(std::pair<std::string, int*>("GraphicsScale", &graphics_scale_));
	option_name_.insert(std::pair<std::string, int*>("TransparentBufferSize", &transparent_buffer_size_));
	option_name_.insert(std::pair<std::string, int*>("SolidBufferSize", &solid_buffer_size_));
	option_name_.insert(std::pair<std::string, int*>("LightEngineThreads", &light_engine_threads_));
}

void Options::GenerateOptionFile() { //Generate file if deleted
	FileManager::CreateFile("options.txt");

	std::ofstream file("options.txt");

	for (const auto& option : option_name_) {
		std::string str = option.first + ":" + std::to_string((*option.second));
		file << str << "\n";
	}

	file.close();
	g_logger.LogDebug("Option", "Generated option file");
}