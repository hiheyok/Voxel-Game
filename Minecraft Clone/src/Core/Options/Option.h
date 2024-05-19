#pragma once
#include "../../FileManager/Files.h"
#include "../../Utils/LogUtils.h"
#include <iostream>

class Options {
public:
	int VerticalRenderDistance = 8;
	int HorizontalRenderDistance = 16;
	int WorldGenThreads = 6;
	int MeshThreads = 6;
	int GraphicsScale = 2;
	int TransparentBufferSize = 1000000000;
	int SolidBufferSize = 2000000000;

	Options() {
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
private:

	bool SetValue(std::string name, std::string value) {
		int val = stoi(value);

		if (!OptionName.count(name)) {
			Logger.LogError("Option", "Unknown option: " + name);
			return false;
		}

		*OptionName[name] = val;
		return true;
	}

	void ProcessTokens(std::vector<std::string> tokens) {
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

	void SetOptionNameTable() {
		OptionName.insert(std::pair<std::string, int*>("HorizontalRenderDistance", &HorizontalRenderDistance));
		OptionName.insert(std::pair<std::string, int*>("VerticalRenderDistance", &VerticalRenderDistance));
		OptionName.insert(std::pair<std::string, int*>("WorldGenThreads", &WorldGenThreads));
		OptionName.insert(std::pair<std::string, int*>("MeshThreads", &MeshThreads));
		OptionName.insert(std::pair<std::string, int*>("GraphicsScale", &GraphicsScale));
		OptionName.insert(std::pair<std::string, int*>("TransparentBufferSize", &TransparentBufferSize));
		OptionName.insert(std::pair<std::string, int*>("SolidBufferSize", &SolidBufferSize));
	}

	void GenerateOptionFile() { //Generate file if deleted
		FileManager::CreateFile("options.txt");

		std::ofstream file("options.txt");

		for (const auto& Option : OptionName) {
			std::string str = Option.first + ":" + std::to_string((*Option.second));
			file << str << "\n";
		}

		file.close();
	}

	std::unordered_map<std::string, int*> OptionName; //name -> ptr

} extern AppOptions;