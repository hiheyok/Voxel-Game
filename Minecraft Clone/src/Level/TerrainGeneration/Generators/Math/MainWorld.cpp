// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include <memory>

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/TerrainGeneration/Generators/Math/MathWorld.h"

MathWorld::MathWorld(GameContext& game_context)
    : WorldGenerator{game_context} {}
MathWorld::~MathWorld() = default;

void MathWorld::Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) {
  int cx = pos.x * kChunkDim;
  int cy = pos.y * kChunkDim;
  int cz = pos.z * kChunkDim;
  float zoom = 20.f;

  for (auto [x, y, z] : Product<3>(kChunkDim)) {
    BlockPos block_pos{x, y, z};

    if ((chunk->GetBlock(block_pos) ==
         game_context_.blocks_->RED_STAINED_GLASS) ||
        (chunk->GetBlock(block_pos) ==
         game_context_.blocks_->BLACK_STAINED_GLASS))
      continue;

    glm::vec3 globalPos(cx + block_pos.x, cy + block_pos.y, cz + block_pos.z);

    float gx = globalPos.x / zoom;
    float gy = globalPos.y / zoom;
    float gz = globalPos.z / zoom;

    if (gz * gz + gy * gy > 4) {
      chunk->SetBlock(game_context_.blocks_->BRICKS, block_pos);
    }

    int Unit = 1;
    int UnitSize = 5;

    if (gx == 0 && gy == 0) {
      if ((static_cast<int>(globalPos.z) % (Unit * static_cast<int>(zoom))) ==
          0) {
        for (auto [i, j] : Product<2>(-UnitSize, UnitSize + 1)) {
          chunk->SetBlock(game_context_.blocks_->RED_STAINED_GLASS,
                          block_pos + BlockPos{j, i, 0});
        }
      } else {
        chunk->SetBlock(game_context_.blocks_->BLACK_STAINED_GLASS, block_pos);
      }
    }
    if (gz == 0 && gy == 0) {
      if ((static_cast<int>(globalPos.x) % (Unit * static_cast<int>(zoom))) ==
          0) {
        for (auto [i, j] : Product<2>(-UnitSize, UnitSize + 1)) {
          chunk->SetBlock(game_context_.blocks_->RED_STAINED_GLASS,
                          block_pos + BlockPos{0, j, i});
        }
      } else {
        chunk->SetBlock(game_context_.blocks_->BLACK_STAINED_GLASS, block_pos);
      }
    }
    if (gz == 0 && gx == 0) {
      if ((static_cast<int>(globalPos.y) % (Unit * static_cast<int>(zoom))) ==
          0) {
        for (auto [i, j] : Product<2>(-UnitSize, UnitSize + 1)) {
          chunk->SetBlock(game_context_.blocks_->RED_STAINED_GLASS,
                          block_pos + BlockPos{i, 0, j});
        }
      } else {
        chunk->SetBlock(game_context_.blocks_->BLACK_STAINED_GLASS, block_pos);
      }
    }
  }
}