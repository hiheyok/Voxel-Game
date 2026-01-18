// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Chunk/Chunk.h"

#include <array>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/ChunkData.h"
#include "Level/TerrainGeneration/Structures/Structure.h"

using std::move;

Chunk::Chunk(GameContext& context) : ChunkContainer{context} {}
Chunk::~Chunk() = default;
Chunk::Chunk(GameContext& context, ChunkRawData&& data)
    : ChunkContainer(context, move(data)) {
  UpdateGen();
}
void Chunk::UpdateGen() {
  for (auto offset : Directions<ChunkPos>()) {
    if (!neighbors_[offset]) {
      continue;
    }

    ChunkContainer* chunk = neighbors_[offset];

    if (chunk->outside_block_to_place_[!offset].size() == 0) {
      continue;
    }

    for (auto [block_pos, block] : chunk->outside_block_to_place_[!offset]) {
      SetBlock(block, block_pos);
    }

    chunk->outside_block_to_place_[!offset].clear();
  }
}
