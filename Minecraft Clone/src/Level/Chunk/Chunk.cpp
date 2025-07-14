// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Chunk/Chunk.h"

#include <algorithm>
#include <array>
#include <utility>
#include <iostream>

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Heightmap/Heightmap.h"
#include "Level/Light/LightStorage.h"
#include "Level/TerrainGeneration/Structures/Structure.h"

Chunk::Chunk(GameContext& game_context) : ChunkContainer{game_context} {}
Chunk::~Chunk() = default;
Chunk::Chunk(GameContext& game_context, const ChunkRawData& data)
    : ChunkContainer{game_context, data} {}

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
