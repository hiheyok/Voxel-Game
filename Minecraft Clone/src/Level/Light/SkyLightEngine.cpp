#include "Level/Light/SkyLightEngine.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/Heightmap/Heightmap.h"
#include "Level/Light/ChunkLightTask.h"
#include "Level/Light/LightEngine.h"
#include "Level/Light/LightEngineCache.h"
#include "Level/World/WorldInterface.h"
#include "Utils/Assert.h"
#include "Utils/Iterators.h"

SkyLightEngine::SkyLightEngine(GameContext& context, WorldInterface& world)
    : LightEngine{context, world} {
  heightmap_block_change_.fill(INT8_MIN);
  type_ = EngineType::kSkyLight;
}

SkyLightEngine::~SkyLightEngine() = default;

void SkyLightEngine::LightChunk(ChunkPos chunk_pos) {
  GAME_ASSERT(light_cache_ != nullptr, "Light cache is null");
  GAME_ASSERT(CheckChunk(chunk_pos), "Chunk not found");

  Chunk* chunk = GetChunk(chunk_pos);
  BlockPos chunk_offset = chunk_pos.GetBlockPosOffset();
  const HeightMap& heightmap = *chunk->heightmap_.get();
  InternalTask task;

  for (auto [x, z] : Product<2>(kChunkDim)) {
    int height = heightmap.Get(x, z);

    // Attempts to cast shadow below
    if (height != -1) {
      BlockPos pos = BlockPos{x, -1, z} + chunk_offset;
      TryPropagateShadow(pos);
    }

    // If column is entirely covered
    if (height >= kChunkDim - 1) {
      continue;
    }

    for (int y = height + 1; y < kChunkDim; ++y) {
      task.block_pos_ = BlockPos{x, y, z} + chunk_offset;
      task.direction_ = kAllDirections;
      task.light_lvl_ = kMaxLightLevel;
      EnqueueIncrease(task);
    }
  }

  CheckNeighborChunk(chunk_pos);
  DelayDecrease();
  DelayIncrease();
  PropagateDecrease();
  ResetDecreaseQueue();
  ResetIncreaseQueue();
  light_cache_ = nullptr;
}
void SkyLightEngine::CheckNeighborChunk(ChunkPos center_chunk_pos) {
  GAME_ASSERT(CheckChunk(center_chunk_pos), "Center chunk not found");
  Chunk* center_chunk = GetChunk(center_chunk_pos);

  InternalTask task;
  for (auto direction : Directions<ChunkPos>()) {
    if (direction == kDownDirection) {
      continue;
    }

    ChunkPos curr_chunk_pos = center_chunk_pos + direction;
    BlockPos curr_chunk_offset = curr_chunk_pos.GetBlockPosOffset();

    if (!light_cache_->CheckChunkHasLighting(curr_chunk_pos)) {
      continue;
    }

    int axis = direction.GetAxis();
    int axis_u = (axis + 1) % 3;
    int axis_v = (axis + 2) % 3;

    if (axis == kYAxis) {
      std::swap(axis_u, axis_v);  // For better cache access pattern
    }

    BlockPos curr_pos;
    curr_pos = curr_chunk_offset;
    curr_pos[axis] = direction.IsNegative() * (kChunkDim - 1);

    for (auto [u, v] : Product<2>(kChunkDim)) {
      curr_pos[axis_u] = u;
      curr_pos[axis_v] = v;

      int curr_light = GetLightLvl(curr_pos);

      if (curr_light <= 1) {
        continue;
      }

      BlockPos next_pos = curr_pos;
      next_pos.IncrementSide(direction.GetOppositeDirection(), 1);
      BlockID neigh_block = GetBlock(next_pos);

      int opacity = properties_[neigh_block].opacity_;
      int spread_lvl = curr_light - std::max(1, opacity);
      if (spread_lvl <= 0) {
        continue;
      }

      task.block_pos_ = next_pos;
      task.light_lvl_ = static_cast<uint8_t>(spread_lvl);
      task.direction_ = direction;
      EnqueueIncrease(task);
    }
  }
}

void SkyLightEngine::CheckBlock(BlockPos block_pos) {
  int curr_light_lvl = GetLightLvl(block_pos);

  if (curr_light_lvl == kMaxLightLevel) {
    // Propagate the light back
    InternalTask task;
    task.block_pos_ = block_pos;
    task.direction_ = kAllDirections;
    task.light_lvl_ = static_cast<uint8_t>(curr_light_lvl);
    EnqueueIncrease(task);
    SetLightLvl(block_pos, curr_light_lvl);
  } else {
    SetLightLvl(block_pos, 0);
  }

  InternalTask task;
  task.block_pos_ = block_pos;
  task.direction_ = kAllDirections;
  task.light_lvl_ = static_cast<uint8_t>(curr_light_lvl);
  EnqueueDecrease(task);
}

void SkyLightEngine::PropagateChanges(const ChunkLightTask& tasks) {
  // Setup heightmap
  for (const auto& task : tasks.GetLightTask()) {
    const size_t idx = task.x * kChunkDim + task.z;
    heightmap_block_change_[idx] =
        std::max(heightmap_block_change_[idx], static_cast<int8_t>(task.y));
  }

  BlockPos chunk_offset = tasks.GetChunkPos().GetBlockPosOffset();

  size_t idx = 0;
  const int x_beg = chunk_offset.x;
  const int z_beg = chunk_offset.z;

  const int x_end = chunk_offset.x + kChunkDim;
  const int z_end = chunk_offset.z + kChunkDim;

  for (auto [x, z] : Product<2>({{x_beg, x_end}, {z_beg, z_end}})) {
    if (heightmap_block_change_[idx] == INT8_MIN) {
      idx++;
      continue;  // Nothing is changed
    }

    int max_y = heightmap_block_change_[idx];
    heightmap_block_change_[idx] = INT8_MIN;

    // Cast a shadow
    int y = max_y + chunk_offset.y;
    BlockPos pos = {x, y, z};
    int max_propagation = TryPropagateSkylight(pos);
    pos.y = max_propagation;

    TryPropagateShadow(pos);

    // Instead of multiplying to get index, increment by 1 instead
    idx++;  // Propagation loops through z axis first then x axis
  }

  DelayIncrease();
  DelayDecrease();

  // Then check blocks now
  for (const auto& task : tasks.GetLightTask()) {
    CheckBlock(task.GetLocalPos() + chunk_offset);
  }

  PropagateDecrease();
}

void SkyLightEngine::DelayDecrease() {
  for (size_t i = 0; i < enqueue_decrease_pos_; ++i) {
    InternalTask& task = decrease_queue_[i];
    SetLightLvl(task.block_pos_, 0);
  }
}

void SkyLightEngine::DelayIncrease() {
  for (size_t i = 0; i < enqueue_increase_pos_; ++i) {
    InternalTask& task = increase_queue_[i];
    SetLightLvl(task.block_pos_, task.light_lvl_);
  }
}

int SkyLightEngine::TryPropagateSkylight(BlockPos block_pos) {
  if (!light_cache_->CheckChunk(block_pos)) {
    return block_pos.y;
  }

  BlockPos block_top_pos = block_pos;
  ++block_top_pos.y;

  // Check if the top of the column is covered or theres no chunk above
  if (light_cache_->CheckChunk(block_top_pos) &&
      GetLightLvl(block_top_pos) != kMaxLightLevel) {
    return block_pos.y;
  }

  Chunk* curr_chunk = light_cache_->GetChunk(block_pos);
  BlockID curr_block;

  int local_x = block_pos.x & (kChunkDim - 1);
  int local_z = block_pos.z & (kChunkDim - 1);

  // Terminates if it hits an opaque block or bottom of world
  InternalTask task;
  task.direction_ = kAllDirections;
  task.light_lvl_ = kMaxLightLevel;

  while (true) {
    // Use heightmap to optimize finding opaque block
    int height = curr_chunk->heightmap_->Get(local_x, local_z);
    int bot_y = block_pos.ToChunkPos().y * kChunkDim;
    for (; block_pos.y > bot_y + height; --block_pos.y) {
      task.block_pos_ = block_pos;
      EnqueueIncrease(task);
    }

    if (height != -1) {
      break;
    }

    if (!light_cache_->CheckChunk(block_pos)) {
      break;
    }
  }

  return block_pos.y;
}

void SkyLightEngine::TryPropagateShadow(BlockPos block_pos) {
  if (!light_cache_->CheckChunk(block_pos)) {
    return;
  }

  // Check if the column is already in a shadow
  BlockPos block_top_pos = block_pos;
  ++block_top_pos.y;
  if (light_cache_->CheckChunk(block_top_pos) &&
      GetLightLvl(block_top_pos) == 0) {
    return;
  }

  Chunk* curr_chunk = light_cache_->GetChunk(block_pos);
  int curr_opacity = 0;
  int curr_light_lvl;

  // Terminate if it goes out of world or hit block with an opacity > 0
  while (true) {
    curr_light_lvl = GetLightLvl(block_pos);
    if (curr_light_lvl == 0) break;
    InternalTask task;
    task.block_pos_ = block_pos;
    task.direction_ = kAllDirections;
    task.light_lvl_ = static_cast<uint8_t>(curr_light_lvl);
    EnqueueDecrease(task);

    --block_pos.y;
    if ((block_pos.y & (kChunkDim - 1)) ==
        (kChunkDim - 1)) {  // Enters new chunk
      if (!light_cache_->CheckChunk(block_pos)) break;
      curr_chunk = light_cache_->GetChunk(block_pos);
    }
  }
}
