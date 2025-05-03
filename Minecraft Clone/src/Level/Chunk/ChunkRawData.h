// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Chunk/Palette.h"
#include "Level/Light/LightStorage.h"

struct ChunkRawData {
  ChunkRawData();
  ChunkRawData(const Palette& chunkData, const LightStorage& lightData,
               ChunkPos pos);
  ChunkRawData(const ChunkRawData&);
  ChunkRawData(ChunkRawData&&);
  ~ChunkRawData();

  ChunkRawData& operator=(const ChunkRawData&);
  ChunkRawData& operator=(ChunkRawData&&);

  Palette chunk_data_;
  LightStorage lighting_data_;
  ChunkPos pos_;
};
