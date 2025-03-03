#pragma once
#include "../../FileManager/Files.h"
#include "../../Utils/LogUtils.h"
#include <iostream>
#include "../../Level/Typenames.h"

class Options {
public:
	int vertical_render_distance_ = 8;
	int horizontal_render_distance_ = 16;
	int world_gen_threads_ = 6;
	int mesh_threads_ = 6;
	int graphics_scale_ = 2;
	int transparent_buffer_size_ = 1000000000;
	int solid_buffer_size_ = 2000000000;
	int light_engine_threads_ = 2;

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

	FastHashMap<std::string, int*> option_name_; //name -> ptr

} extern g_app_options;