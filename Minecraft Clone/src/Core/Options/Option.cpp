// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Core/Options/Option.h"

#include <string>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "FileManager/Files.h"
#include "Utils/LogUtils.h"

Options::Options(GameContext& game_context) : game_context_{game_context} {
  SetOptionNameTable();

  // Set values from option file
  if (!FileManager::CheckFile("options.txt")) {
    GenerateOptionFile();
    return;  // check if option file exist
  }

  File file = FileManager::GetFile("options.txt");

  std::vector<std::string> tokens = file.GetToken(':');

  ProcessTokens(tokens);
}

bool Options::SetValue(std::string name, std::string value) {
  size_t val = stoll(value);

  if (!option_name_.count(name)) {
    game_context_.logger_->LogError("Options::SetValue",
                                    "Unknown option: " + name);
    return false;
  }

  *option_name_[name] = val;
  return true;
}

void Options::ProcessTokens(std::vector<std::string> tokens) {
  bool success = true;

  for (int i = 0; i < static_cast<int>(tokens.size() / 2); i++) {
    std::string name = tokens[2 * i];
    std::string val = tokens[2 * i + 1];
    game_context_.logger_->LogDebug("Options::ProcessTokens", name + ":" + val);
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
    std::string str = option.first + ":" + std::to_string((*option.second));
    file << str << "\n";
  }

  file.close();
  game_context_.logger_->LogDebug("Options::GenerateOptionFile",
                                  "Generated option file");
}
