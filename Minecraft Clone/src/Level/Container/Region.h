// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>
#include <vector>

#include "Core/Typenames.h"

class Chunk;

/*
 * Simple chunk storage for the client
 */

class Region {
 public:
  Region();
  ~Region();

  Chunk* GetChunk(ChunkPos pos) const;
  void InsertChunk(std::unique_ptr<Chunk> chunk);
  void EraseChunk(ChunkPos pos);
  bool CheckChunk(ChunkPos pos) const;
  int GetChunkCount() const;
  void IncrementUsage();
  size_t GetUsageCount() const;
  void ResetUsageCount();

 private:
  int chunk_count_;
  int usage_;  // use for finding region with the most usage and caching them
               // for faster access
  std::vector<std::unique_ptr<Chunk>> region_data_;
};
