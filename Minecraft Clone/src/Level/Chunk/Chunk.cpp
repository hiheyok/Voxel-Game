// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Chunk/Chunk.h"

#include "Level/Light/LightStorage.h"
#include "Level/TerrainGeneration/Structures/Structure.h"

Chunk::Chunk() = default;
Chunk::~Chunk() = default;
Chunk::Chunk(const ChunkRawData& data) : ChunkContainer{data} {}

void Chunk::UpdateGen() {
  for (const auto& offset : Directions<ChunkPos>()) {
    if (!neighbors_[offset].has_value() ||
        neighbors_[offset].value()->outside_block_to_place_[!offset].size() ==
            0) {
      continue;
    }

    for (const auto& block :
         neighbors_[offset].value()->outside_block_to_place_[!offset]) {
      SetBlock(block.block_, block.pos_);
    }

    neighbors_[offset].value()->outside_block_to_place_[!offset].clear();
  }
}
