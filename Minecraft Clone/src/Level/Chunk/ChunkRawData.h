// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Chunk/Palette.h"
#include "Level/Light/LightStorage.h"

struct ChunkRawData {
  ChunkRawData(const Palette& chunkData, const LightStorage& sky_light, const LightStorage& block_light,
               ChunkPos pos);
  ChunkRawData(const ChunkRawData&);
  ChunkRawData(ChunkRawData&&);
  ~ChunkRawData();

  ChunkRawData& operator=(const ChunkRawData&);
  ChunkRawData& operator=(ChunkRawData&&);

  Palette chunk_data_;
  LightStorage sky_light_;
  LightStorage block_light_;
  ChunkPos pos_;
};
