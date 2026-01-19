#include "Level/Light/BlockLightEngine.h"

#include <algorithm>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Light/ChunkLightTask.h"
#include "Level/Light/LightEngine.h"
#include "Level/Light/LightEngineCache.h"
#include "Level/World/WorldInterface.h"
#include "Utils/Assert.h"
#include "Utils/Iterators.h"

BlockLightEngine::BlockLightEngine(GameContext& context, WorldInterface& world)
    : LightEngine{context, world} {
  type_ = EngineType::kBlockLight;
}
BlockLightEngine::~BlockLightEngine() = default;

void BlockLightEngine::LightChunk(ChunkPos chunk_pos) {
  GAME_ASSERT(light_cache_ != nullptr, "Light cache is null");
  GAME_ASSERT(CheckChunk(chunk_pos), "Chunk not found");

  Chunk* chunk = GetChunk(chunk_pos);
  InternalTask task;
  task.direction_ = kAllDirections;
  BlockPos chunk_offset = chunk_pos.GetBlockPosOffset();

  int x0 = chunk_offset.x;
  int y0 = chunk_offset.y;
  int z0 = chunk_offset.z;

  int x1 = chunk_offset.x + kChunkDim;
  int y1 = chunk_offset.y + kChunkDim;
  int z1 = chunk_offset.z + kChunkDim;

  for (auto [x, y, z] : Product<3>({{x0, x1}, {y0, y1}, {z0, z1}})) {
    BlockPos block_pos{x, y, z};
    BlockID block = GetBlock(block_pos);

    int block_emission = properties_[block].light_emission;
    if (block_emission > 0) {
      task.block_pos_ = block_pos;
      task.light_lvl_ = static_cast<uint8_t>(block_emission);
      increase_queue_.Push(task);
      SetLightLvl(block_pos, block_emission);
    }
  }

  CheckNeighborChunk(chunk_pos);
  PropagateIncrease();
  increase_queue_.Clear();
  light_cache_ = nullptr;
}
void BlockLightEngine::CheckNeighborChunk(ChunkPos center_chunk_pos) {
  GAME_ASSERT(CheckChunk(center_chunk_pos), "Center chunk not found");

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

    if (axis == kYAxis) {
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

      task.block_pos_ = next_pos;
      task.light_lvl_ = static_cast<uint8_t>(spread_lvl);
      task.direction_ = direction;
      increase_queue_.Push(task);
    }
  }
}

void BlockLightEngine::CheckBlock(BlockPos block_pos) {
  // Get info
  GAME_ASSERT(CheckChunk(block_pos.ToChunkPos()), "Block chunk not found");
  BlockID block = GetBlock(block_pos);
  int emission_lvl = context_.blocks_->GetBlockProperties(block).light_emission;
  int curr_lvl = GetLightLvl(block_pos);

  SetLightLvl(block_pos, emission_lvl);
  if (emission_lvl != 0) {
    InternalTask task;
    task.block_pos_ = block_pos;
    task.direction_ = kAllDirections;
    task.light_lvl_ = static_cast<uint8_t>(emission_lvl);
    increase_queue_.Push(task);
  }

  InternalTask task;
  task.block_pos_ = block_pos;
  task.light_lvl_ = static_cast<uint8_t>(curr_lvl);
  task.direction_ = kAllDirections;
  decrease_queue_.Push(task);
}

void BlockLightEngine::PropagateChanges(const ChunkLightTask& tasks) {
  BlockPos chunk_offset = tasks.GetChunkPos().GetBlockPosOffset();
  for (auto task : tasks.GetLightTask()) {
    CheckBlock(task.GetLocalPos() + chunk_offset);
  }

  PropagateDecrease();
}