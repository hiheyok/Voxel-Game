#include "Level/Light/BlockLightEngine.h"

#include <cassert>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/Heightmap/Heightmap.h"
#include "Level/Light/ChunkLightTask.h"
#include "Level/Light/LightEngineCache.h"
#include "Level/World/WorldInterface.h"

BlockLightEngine::BlockLightEngine(GameContext& game_context,
                                   WorldInterface& world)
    : LightEngine{game_context, world} {}
BlockLightEngine::~BlockLightEngine() = default;

void BlockLightEngine::LightChunk(ChunkPos chunk_pos) {
  assert(light_cache_ != nullptr);
  assert(CheckChunk(chunk_pos));
  Chunk* chunk = GetChunk(chunk_pos);

  for (int x = 0; x < kChunkDim; ++x) {
    for (int y = 0; y < kChunkDim; ++y) {
      for (int z = 0; z < kChunkDim; ++z) {
        BlockID block = chunk->GetBlockUnsafe(BlockPos{x, y, z});
        int block_emission =
            game_context_.blocks_->GetBlockProperties(block).light_emission;
        if (block_emission > 0) {
          InternalTask task;
          task.SetBlockPos(BlockPos{x, y, z});
          task.SetChunkPos(chunk_pos);
          task.SetLightLevel(block_emission);
          task.SetDirection(kAllDirections);
          EnqueueIncrease(task);
          SetLightLvl(chunk_pos.GetBlockPosOffset() + BlockPos{x, y, z}, block_emission);
        }
      }
    }
  }

  CheckNeighborChunk(chunk_pos);
  PropagateIncrease();
  ResetIncreaseQueue();
  light_cache_ = 0;
}
void BlockLightEngine::CheckNeighborChunk(ChunkPos center_chunk_pos) {
  assert(CheckChunk(center_chunk_pos));
  Chunk* center_chunk = GetChunk(center_chunk_pos);

  for (const auto& direction : Directions<ChunkPos>()) {
    auto neighbor_chunk = center_chunk->GetNeighbor(direction);

    if (!neighbor_chunk.has_value() || !neighbor_chunk.value()->IsLightUp()) {
      continue;
    }

    const LightStorage& neigh_light =
        *neighbor_chunk.value()->block_light_.get();

    int axis = direction.GetAxis();
    int axis_u = (axis + 1) % 3;
    int axis_v = (axis + 2) % 3;

    if (axis == Directions<ChunkPos>::kYAxis) {
      std::swap(axis_u, axis_v);  // For better cache access pattern
    }

    BlockPos curr_pos;
    curr_pos[axis] = direction.IsNegative() * (kChunkDim - 1);

    for (int u = 0; u < kChunkDim; ++u) {
      for (int v = 0; v < kChunkDim; ++v) {
        curr_pos[axis_u] = u;
        curr_pos[axis_v] = v;

        int curr_light = neigh_light.GetLighting(curr_pos);

        if (curr_light <= 1) {
          continue;
        }

        BlockPos next_pos = curr_pos;
        next_pos[axis] = direction.IsPositive() * (kChunkDim - 1);

        BlockID neigh_block = center_chunk->GetBlockUnsafe(next_pos);
        int opacity =
            game_context_.blocks_->GetBlockProperties(neigh_block).opacity_;
        int spread_lvl = curr_light - std::max(1, opacity);
        if (spread_lvl <= 0) {
          continue;
        }

        InternalTask task;
        task.SetBlockPos(next_pos);
        task.SetChunkPos(center_chunk_pos);
        task.SetLightLevel(spread_lvl);
        task.SetDirection(direction);
        EnqueueIncrease(task);
      }
    }
  }
}
void BlockLightEngine::SetLightLvl(BlockPos block_pos, int light_lvl) {
  return light_cache_->SetBlockLight(block_pos, light_lvl);
}
int BlockLightEngine::GetLightLvl(BlockPos block_pos) {
  return light_cache_->GetBlockLight(block_pos);
}
void BlockLightEngine::CheckBlock(ChunkPos chunk_pos, BlockPos block_pos) {
  // Get info
  assert(CheckChunk(chunk_pos));
  BlockPos global_pos = block_pos + chunk_pos.GetBlockPosOffset();
  BlockID block = GetBlock(global_pos);
  int emission_lvl =
      game_context_.blocks_->GetBlockProperties(block).light_emission;
  int curr_lvl = GetLightLvl(global_pos);

  SetLightLvl(global_pos, emission_lvl);
  if (emission_lvl != 0) {
    InternalTask task;
    task.SetBlockPos(block_pos);
    task.SetChunkPos(chunk_pos);
    task.SetDirection(kAllDirections);
    task.SetLightLevel(emission_lvl);
    EnqueueIncrease(task);
  }

  InternalTask task;
  task.SetBlockPos(block_pos);
  task.SetChunkPos(chunk_pos);
  task.SetLightLevel(curr_lvl);
  task.SetDirection(kAllDirections);
  EnqueueDecrease(task);
}

void BlockLightEngine::PropagateChanges(const ChunkLightTask& tasks) {
  for (const auto& task : tasks.GetLightTask()) {
    CheckBlock(tasks.GetChunkPos(), task.GetLocalPos());
  }

  PropagateDecrease();
}