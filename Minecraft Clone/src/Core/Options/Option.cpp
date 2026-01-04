// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Core/Options/Option.h"

#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "FileManager/Files.h"
#include "Utils/LogUtils.h"

using std::string;
using std::to_string;
using std::vector;

Options::Options(GameContext& context) : context_{context} {
  SetOptionNameTable();

  // Set values from option file
  if (!FileManager::CheckFile("options.txt")) {
    GenerateOptionFile();
    return;  // check if option file exist
  }

  File file = FileManager::GetFile("options.txt");

  vector<string> tokens = file.GetToken(':');

  ProcessTokens(tokens);
}

bool Options::SetValue(string name, string value) {
  size_t val = stoll(value);

  if (!option_name_.count(name)) {
    LOG_ERROR("Unknown option: {}", name);
    return false;
  }

  *option_name_[name] = val;
  return true;
}

void Options::ProcessTokens(vector<string> tokens) {
  bool success = true;

  for (size_t i = 0; i < tokens.size() / 2; i++) {
    string name = tokens[2 * i];
    string val = tokens[2 * i + 1];
    LOG_DEBUG("{}:{}", name, val);
    if (!SetValue(name, val)) {
      success = false;
    }
  }
  // Regenerate file if it has an error in it
  if (!success) {
    FileManager::DeleteFile("options.txt");
    GenerateOptionFile();
  }
}

void Options::SetOptionNameTable() {
  option_name_["HorizontalRenderDistance"] = &horizontal_render_distance_;
  option_name_["VerticalRenderDistance"] = &vertical_render_distance_;
  option_name_["WorldGenThreads"] = &world_gen_threads_;
  option_name_["MeshThreads"] = &mesh_threads_;
  option_name_["GraphicsScale"] = &graphics_scale_;
  option_name_["TransparentBufferSize"] = &transparent_buffer_size_;
  option_name_["SolidBufferSize"] = &solid_buffer_size_;
  option_name_["LightEngineThreads"] = &light_engine_threads_;
}

void Options::GenerateOptionFile() {  // Generate file if deleted
  FileManager::CreateFile("options.txt");

  std::ofstream file("options.txt");

  for (const auto& option : option_name_) {
    string str = option.first + ":" + to_string((*option.second));
    file << str << "\n";
  }

  file.close();
  LOG_DEBUG("Generated option file");
}
