// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <vector>

/*
Vertices Format
- 2 floats for relative position in screen
- 2 float for UV
- 1 float for TextureID
*/

struct DisplayVertices {
  std::vector<float> vertices_;
  std::vector<uint32_t> indices_;
};
