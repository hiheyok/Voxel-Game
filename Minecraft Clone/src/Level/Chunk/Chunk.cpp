// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Chunk/Chunk.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Heightmap/Heightmap.h"
#include "Level/Light/LightStorage.h"
#include "Level/TerrainGeneration/Structures/Structure.h"

Chunk::Chunk(GameContext& context) : ChunkContainer{context} {}
Chunk::~Chunk() = default;
Chunk::Chunk(GameContext& context, const ChunkRawData& data)
    : ChunkContainer{context, data} {}

void Chunk::UpdateGen() {
  for (auto offset : Directions<ChunkPos>()) {
    if (!neighbors_[offset].has_value()) {
      continue;
    }

    ChunkContainer* chunk = neighbors_[offset].value();

    if (chunk->outside_block_to_place_[!offset].size() == 0) {
      continue;
    }

    for (auto [block_pos, block] : chunk->outside_block_to_place_[!offset]) {
      SetBlock(block, block_pos);
    }

    chunk->outside_block_to_place_[!offset].clear();
  }
}
