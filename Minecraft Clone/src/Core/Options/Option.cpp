#include "Option.h"

bool Options::SetValue(std::string name, std::string value) {
	int val = stoi(value);

	if (!OptionName.count(name)) {
		Logger.LogError("Option", "Unknown option: " + name);
		return false;
	}

	*OptionName[name] = val;
	return true;
}

void Options::ProcessTokens(std::vector<std::string> tokens) {
	bool Success = true;

	for (int i = 0; i < (tokens.size() / 2); i++) {
		std::string name = tokens[2 * i];
		std::string val = tokens[2 * i + 1];
		if (!SetValue(name, val)) {
			Success = false;
		}
	}
	//Regenerate file if it has an error in it
	if (!Success) {
		FileManager::DeleteFile("options.txt");
		GenerateOptionFile();
	}
}

void Options::SetOptionNameTable() {
	OptionName.insert(std::pair<std::string, int*>("HorizontalRenderDistance", &horizontal_render_distance_));
	OptionName.insert(std::pair<std::string, int*>("VerticalRenderDistance", &vertical_render_distance_));
	OptionName.insert(std::pair<std::string, int*>("WorldGenThreads", &WorldGenThreads));
	OptionName.insert(std::pair<std::string, int*>("MeshThreads", &MeshThreads));
	OptionName.insert(std::pair<std::string, int*>("GraphicsScale", &GraphicsScale));
	OptionName.insert(std::pair<std::string, int*>("TransparentBufferSize", &TransparentBufferSize));
	OptionName.insert(std::pair<std::string, int*>("SolidBufferSize", &SolidBufferSize));
	OptionName.insert(std::pair<std::string, int*>("LightEngineThreads", &LightEngineThreads));
}

void Options::GenerateOptionFile() { //Generate file if deleted
	FileManager::CreateFile("options.txt");

	std::ofstream file("options.txt");

	for (const auto& Option : OptionName) {
		std::string str = Option.first + ":" + std::to_string((*Option.second));
		file << str << "\n";
	}

	file.close();
}