// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <optional>
#include <string>
#include <vector>

#include "RenderEngine/OpenGL/Texture/Texture.h"

class TextureArray : public Texture {
 public:
  explicit TextureArray(GameContext&);
  ~TextureArray();

  void LoadToGPU();

  void SetSize(int width, int height);

  void AddData(std::vector<uint8_t> data, size_t width, size_t height,
               int format);

  bool AddTextureToArray(RawTextureData* data);

  std::optional<RawTextureData> AddTextureToArray(std::string file);

  int GetLayers() const;

  std::vector<uint8_t> array_data_;

  int layers_ = 0;
};
