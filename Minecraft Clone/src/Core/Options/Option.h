#pragma once
#include "../../FileManager/Files.h"
#include "../../Utils/LogUtils.h"
#include <iostream>
#include "../../Level/Typenames.h"

class Options {
public:
	int VerticalRenderDistance = 8;
	int HorizontalRenderDistance = 16;
	int WorldGenThreads = 6;
	int MeshThreads = 6;
	int GraphicsScale = 2;
	int TransparentBufferSize = 1000000000;
	int SolidBufferSize = 2000000000;
	int LightEngineThreads = 2;

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

	bool SetValue(std::string name, std::string value);

	void ProcessTokens(std::vector<std::string> tokens);

	void SetOptionNameTable();

	void GenerateOptionFile(); //Generate file if deleted

	FastHashMap<std::string, int*> OptionName; //name -> ptr

} extern AppOptions;