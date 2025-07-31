#include "Level/Light/SkyLightEngine.h"

#include <cassert>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/Heightmap/Heightmap.h"
#include "Level/Light/ChunkLightTask.h"
#include "Level/Light/LightEngineCache.h"
#include "Level/World/WorldInterface.h"

SkyLightEngine::SkyLightEngine(GameContext& game_context, WorldInterface& world)
    : LightEngine{game_context, world} {
  heightmap_block_change_.fill(INT8_MIN);
  type_ = EngineType::kSkyLight;
}

SkyLightEngine::~SkyLightEngine() = default;

void SkyLightEngine::LightChunk(ChunkPos chunk_pos) {
  assert(light_cache_ != nullptr);
  assert(CheckChunk(chunk_pos));

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
      task.SetBlockPos(BlockPos{x, y, z} + chunk_offset);
      task.SetDirection(kAllDirections);
      task.SetLightLevel(kMaxLightLevel);
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
  assert(CheckChunk(center_chunk_pos));
  Chunk* center_chunk = GetChunk(center_chunk_pos);

  InternalTask task;
  for (auto direction : Directions<ChunkPos>()) {
    if (direction == Directions<ChunkPos>::kDown) {
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

    if (axis == Directions<ChunkPos>::kYAxis) {
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

      task.SetBlockPos(next_pos);
      task.SetLightLevel(spread_lvl);
      task.SetDirection(direction);
      EnqueueIncrease(task);
    }
  }
}

void SkyLightEngine::CheckBlock(BlockPos block_pos) {
  BlockID block = GetBlock(block_pos);
  int curr_light_lvl = GetLightLvl(block_pos);

  if (curr_light_lvl == kMaxLightLevel) {
    // Propagate the light back
    InternalTask task;
    task.SetBlockPos(block_pos);
    task.SetDirection(kAllDirections);
    task.SetLightLevel(curr_light_lvl);
    EnqueueIncrease(task);
    SetLightLvl(block_pos, curr_light_lvl);
  } else {
    SetLightLvl(block_pos, 0);
  }

  InternalTask task;
  task.SetBlockPos(block_pos);
  task.SetDirection(kAllDirections);
  task.SetLightLevel(curr_light_lvl);
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

  for (auto [x, z] : Product<2>(kChunkDim)) {
    const size_t idx = x * kChunkDim + z;
    if (heightmap_block_change_[idx] == INT8_MIN) {
      continue;  // Nothing is changed
    }

    int max_y = heightmap_block_change_[idx];
    heightmap_block_change_[idx] = INT8_MIN;

    // Cast a shadow
    int y = max_y;
    BlockPos pos = BlockPos{x, y, z} + chunk_offset;
    int max_propagation = TryPropagateSkylight(pos);
    pos.y = max_propagation;

    TryPropagateShadow(pos);
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
    SetLightLvl(task.GetBlockPos(), 0);
  }
}

void SkyLightEngine::DelayIncrease() {
  for (size_t i = 0; i < enqueue_increase_pos_; ++i) {
    InternalTask& task = increase_queue_[i];
    SetLightLvl(task.GetBlockPos(), task.GetLightLevel());
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
  int curr_opacity = 0;
  BlockID curr_block;

  int local_x = block_pos.x & (kChunkDim - 1);
  int local_z = block_pos.z & (kChunkDim - 1);

  // Terminates if it hits an opaque block or bottom of world
  InternalTask task;
  task.SetDirection(kAllDirections);
  task.SetLightLevel(kMaxLightLevel);

  while (true) {
    // Use heightmap to optimize finding opaque block
    int height = curr_chunk->heightmap_->Get(local_x, local_z);
    int bot_y = block_pos.ToChunkPos().y * kChunkDim;
    for (; block_pos.y > bot_y + height; --block_pos.y) {
      task.SetBlockPos(block_pos);
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
    task.SetBlockPos(block_pos);
    task.SetDirection(kAllDirections);
    task.SetLightLevel(curr_light_lvl);
    EnqueueDecrease(task);

    --block_pos.y;
    if ((block_pos.y & (kChunkDim - 1)) ==
        (kChunkDim - 1)) {  // Enters new chunk
      if (!light_cache_->CheckChunk(block_pos)) break;
      curr_chunk = light_cache_->GetChunk(block_pos);
    }
  }
}
