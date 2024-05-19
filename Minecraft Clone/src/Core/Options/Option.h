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

	Options() {

		//Set values from option file
		if (!FileManager::CheckFile("option.txt")) {
			return; //check if option file exist
		}

		File file = FileManager::GetFile("option.txt");

		std::vector<std::string> tokens = file.GetToken(':');

		SetOptionNameTable();

		ProcessTokens(tokens);
	}
private:

	void SetValue(std::string name, std::string value) {
		int val = stoi(value);

		if (!OptionName.count(name)) {
			Logger.LogError("Option", "Unknown option: " + name);
			return;
		}

		*OptionName[name] = val;
	}

	void ProcessTokens(std::vector<std::string> tokens) {
		for (int i = 0; i < (tokens.size() / 2); i++) {
			std::string name = tokens[2 * i];
			std::string val = tokens[2 * i + 1];
			SetValue(name, val);
			
		}
	}

	void SetOptionNameTable() {
		OptionName.insert(std::pair<std::string, int*>("HorizontalRenderDistance", &HorizontalRenderDistance));
		OptionName.insert(std::pair<std::string, int*>("VerticalRenderDistance", &VerticalRenderDistance));
		OptionName.insert(std::pair<std::string, int*>("WorldGenThreads", &WorldGenThreads));
		OptionName.insert(std::pair<std::string, int*>("MeshThreads", &MeshThreads));
	}

	std::unordered_map<std::string, int*> OptionName; //name -> ptr

} extern AppOptions;