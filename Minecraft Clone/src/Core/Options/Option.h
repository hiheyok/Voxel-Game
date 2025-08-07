// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <string>

#include "Core/Typenames.h"

class GameContext;

class Options {
 public:
  explicit Options(GameContext&);

  size_t vertical_render_distance_ = 8;
  size_t horizontal_render_distance_ = 16;
  size_t world_gen_threads_ = 6;
  size_t mesh_threads_ = 6;
  size_t graphics_scale_ = 2;
  size_t transparent_buffer_size_ = 1000000000;
  size_t solid_buffer_size_ = 2000000000;
  size_t light_engine_threads_ = 2;

 private:
  bool SetValue(std::string name, std::string value);

  void ProcessTokens(std::vector<std::string> tokens);

  void SetOptionNameTable();

  void GenerateOptionFile();  // Generate file if deleted

  GameContext& context_;
  FastHashMap<std::string, size_t*> option_name_;  // name -> ptr
};
