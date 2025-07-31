// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Chunk/Heightmap/Heightmap.h"

#include "Core//Typenames.h"

HeightMap::HeightMap() { data_.fill(kNoBlocks); }
HeightMap::~HeightMap() = default;

void HeightMap::Edit(int x, int z, int height) noexcept {
  data_[x * kChunkDim + z] = static_cast<int8_t>(height);
}

int HeightMap::Get(int x, int z) const noexcept {
  return data_[x * kChunkDim + z];
}

void HeightMap::Clear() { data_.fill(kNoBlocks); }
     