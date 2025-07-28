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
    : LightEngine{game_context, world} {
  type_ = EngineType::kBlockLight;
}
BlockLightEngine::~BlockLightEngine() = default;

void BlockLightEngine::LightChunk(ChunkPos chunk_pos) {
  assert(light_cache_ != nullptr);
  assert(CheckChunk(chunk_pos));

  Chunk* chunk = GetChunk(chunk_pos);
  InternalTask task;
  task.SetDirection(kAllDirections);
  BlockPos chunk_offset = chunk_pos.GetBlockPosOffset();

  for (auto [x, y, z] : Product<3>(kChunkDim)) {
    BlockPos block_pos = BlockPos{x, y, z} + chunk_offset;
    BlockID block = GetBlock(block_pos);

    int block_emission = properties_[block].light_emission;
    if (block_emission > 0) {
      task.SetBlockPos(block_pos);
      task.SetLightLevel(block_emission);
      EnqueueIncrease(task);
      SetLightLvl(block_pos, block_emission);
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
  InternalTask task;

  for (auto direction : Directions<ChunkPos>()) {
    ChunkPos curr_chunk_pos = center_chunk_pos + direction;

    if (!light_cache_->CheckChunkHasLighting(curr_chunk_pos)) {
      continue;
    }

    int axis = direction.GetAxis();
    int axis_u = (axis + 1) % 3;
    int axis_v = (axis + 2) % 3;

    if (axis == Directions<ChunkPos>::kYAxis) {
      std::swap(axis_u, axis_v);  // For better cache access pattern
    }

    int slice_offset = direction.IsNegative() * (kChunkDim - 1);

    BlockPos curr_pos;

    for (auto [u, v] : Product<2>(kChunkDim)) {
      curr_pos = curr_chunk_pos.GetBlockPosOffset();
      curr_pos[axis] += slice_offset;
      curr_pos[axis_u] += u;
      curr_pos[axis_v] += v;

      int curr_light = GetLightLvl(curr_pos);

      if (curr_light <= 1) {
        continue;
      }

      BlockPos next_pos = curr_pos;
      next_pos.IncrementSide(direction.GetOppositeDirection(), 1);
      BlockID next_block = GetBlock(next_pos);

      int opacity = properties_[next_block].opacity_;
      int spread_lvl = curr_light - std::max(1, opacity);
      if (spread_lvl <= 0) {
        continue;
      }

      task.SetBlockPos(next_pos);
      task.SetLightLevel(spread_lvl);
      task.SetDirection(direction);
      EnqueueIncrease(task);
    }
  }
}

void BlockLightEngine::CheckBlock(BlockPos block_pos) {
  // Get info
  assert(CheckChunk(block_pos.ToChunkPos()));
  BlockID block = GetBlock(block_pos);
  int emission_lvl =
      game_context_.blocks_->GetBlockProperties(block).light_emission;
  int curr_lvl = GetLightLvl(block_pos);

  SetLightLvl(block_pos, emission_lvl);
  if (emission_lvl != 0) {
    InternalTask task;
    task.SetBlockPos(block_pos);
    task.SetDirection(kAllDirections);
    task.SetLightLevel(emission_lvl);
    EnqueueIncrease(task);
  }

  InternalTask task;
  task.SetBlockPos(block_pos);
  task.SetLightLevel(curr_lvl);
  task.SetDirection(kAllDirections);
  EnqueueDecrease(task);
}

void BlockLightEngine::PropagateChanges(const ChunkLightTask& tasks) {
  BlockPos chunk_offset = tasks.GetChunkPos().GetBlockPosOffset();
  for (auto task : tasks.GetLightTask()) {
    CheckBlock(task.GetLocalPos() + chunk_offset);
  }

  PropagateDecrease();
}