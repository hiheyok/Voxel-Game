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

  BlockPos block_pos{0, 0, 0};

  for (block_pos.x = 0; block_pos.x < kChunkDim; block_pos.x++) {
    for (block_pos.y = 0; block_pos.y < kChunkDim; block_pos.y++) {
      for (block_pos.z = 0; block_pos.z < kChunkDim; block_pos.z++) {
        if ((chunk->GetBlock(block_pos) ==
             game_context_.blocks_->RED_STAINED_GLASS) ||
            (chunk->GetBlock(block_pos) ==
             game_context_.blocks_->BLACK_STAINED_GLASS))
          continue;

        glm::vec3 globalPos(cx + block_pos.x, cy + block_pos.y,
                            cz + block_pos.z);

        float gx = globalPos.x / zoom;
        float gy = globalPos.y / zoom;
        float gz = globalPos.z / zoom;

        // if (0 < gx * gx - gy * gy - gz) {

        //    SetBlock(Blocks.WHITE_CONCRETE, x, y, z);
        //}

        // if (gy > 3 * gz * gz - gx * gx + 2) {
        //     SetBlock(Blocks.WHITE_CONCRETE, x, y, z);
        // }

        if (gz * gz + gy * gy > 4) {
          chunk->SetBlock(game_context_.blocks_->BRICKS, block_pos);
        }

        int Unit = 1;
        int UnitSize = 5;

        if (gx == 0 && gy == 0) {
          if ((static_cast<int>(globalPos.z) %
               (Unit * static_cast<int>(zoom))) == 0) {
            for (int i = -UnitSize; i <= UnitSize; i++) {
              for (int j = -UnitSize; j <= UnitSize; j++) {
                chunk->SetBlock(game_context_.blocks_->RED_STAINED_GLASS,
                                block_pos + BlockPos{j, i, 0});
              }
            }
          } else {
            chunk->SetBlock(game_context_.blocks_->BLACK_STAINED_GLASS,
                            block_pos);
          }
        }
        if (gz == 0 && gy == 0) {
          if ((static_cast<int>(globalPos.x) %
               (Unit * static_cast<int>(zoom))) == 0) {
            for (int i = -UnitSize; i <= UnitSize; i++) {
              for (int j = -UnitSize; j <= UnitSize; j++) {
                chunk->SetBlock(game_context_.blocks_->RED_STAINED_GLASS,
                                block_pos + BlockPos{0, j, i});
              }
            }
          } else {
            chunk->SetBlock(game_context_.blocks_->BLACK_STAINED_GLASS,
                            block_pos);
          }
        }
        if (gz == 0 && gx == 0) {
          if ((static_cast<int>(globalPos.y) %
               (Unit * static_cast<int>(zoom))) == 0) {
            for (int i = -UnitSize; i <= UnitSize; i++) {
              for (int j = -UnitSize; j <= UnitSize; j++) {
                chunk->SetBlock(game_context_.blocks_->RED_STAINED_GLASS,
                                block_pos + BlockPos{i, 0, j});
              }
            }
          } else {
            chunk->SetBlock(game_context_.blocks_->BLACK_STAINED_GLASS,
                            block_pos);
          }
        }
      }
    }
  }
}
