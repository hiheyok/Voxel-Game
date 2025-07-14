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
}

SkyLightEngine::~SkyLightEngine() = default;

void SkyLightEngine::LightChunk(ChunkPos chunk_pos) {
  assert(light_cache_ != nullptr);
  assert(CheckChunk(chunk_pos));

  Chunk* chunk = GetChunk(chunk_pos);
  const HeightMap& heightmap = *chunk->heightmap_.get();

  for (int x = 0; x < kChunkDim; ++x) {
    for (int z = 0; z < kChunkDim; ++z) {
      int height = heightmap.Get(x, z);

      // If column is entirely covered
      if (height >= kChunkDim - 1) {
        continue;
      }

      for (int y = height + 1; y < kChunkDim; ++y) {
        InternalTask task;
        task.SetBlockPos(BlockPos{x, y, z});
        task.SetChunkPos(chunk_pos);
        task.SetDirection(kAllDirections);
        task.SetLightLevel(kMaxLightLevel);
        EnqueueIncrease(task);
      }
    }
  }

  CheckNeighborChunk(chunk_pos);
  DelayIncrease();
  PropagateIncrease();
  ResetIncreaseQueue();
  light_cache_ = nullptr;
}
void SkyLightEngine::CheckNeighborChunk(ChunkPos center_chunk_pos) {
  assert(CheckChunk(center_chunk_pos));
  Chunk* center_chunk = GetChunk(center_chunk_pos);

  for (const auto& direction : Directions<ChunkPos>()) {
    if (direction == Directions<ChunkPos>::kDown) {
      continue;
    }

    auto neighbor_chunk = center_chunk->GetNeighbor(direction);

    if (!neighbor_chunk.has_value() || !neighbor_chunk.value()->IsLightUp()) {
      continue;
    }

    const LightStorage& neigh_light = *neighbor_chunk.value()->sky_light_.get();

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

void SkyLightEngine::SetLightLvl(BlockPos block_pos, int light_lvl) {
  light_cache_->SetSkyLight(block_pos, light_lvl);
}

int SkyLightEngine::GetLightLvl(BlockPos block_pos) {
  return light_cache_->GetSkyLight(block_pos);
}

void SkyLightEngine::CheckBlock(ChunkPos chunk_pos, BlockPos block_pos) {
  BlockPos global_pos = block_pos + chunk_pos.GetBlockPosOffset();
  BlockID block = GetBlock(global_pos);
  BlockProperties properties = game_context_.blocks_->GetBlockProperties(block);
  int opacity = properties.opacity_;
  int curr_light_lvl = GetLightLvl(global_pos);

  if (curr_light_lvl == kMaxLightLevel) {
    // Propagate the light back
    InternalTask task;
    task.SetBlockPos(block_pos);
    task.SetChunkPos(chunk_pos);
    task.SetDirection(kAllDirections);
    task.SetLightLevel(curr_light_lvl);
    EnqueueIncrease(task);
    SetLightLvl(global_pos, curr_light_lvl);
  } else {
    SetLightLvl(global_pos, 0);
  }

  InternalTask task;
  task.SetBlockPos(block_pos);
  task.SetChunkPos(chunk_pos);
  task.SetDirection(kAllDirections);
  task.SetLightLevel(curr_light_lvl);
  EnqueueDecrease(task);
}

void SkyLightEngine::PropagateChanges(const ChunkLightTask& tasks) {
  // Setup heightmap
  for (const auto& task : tasks.GetLightTask()) {
    heightmap_block_change_[task.x * kChunkDim + task.z] =
        std::max(heightmap_block_change_[task.x * kChunkDim + task.z],
                 static_cast<int8_t>(task.y));
  }

  for (int x = 0; x < kChunkDim; ++x) {
    for (int z = 0; z < kChunkDim; ++z) {
      const int idx = x * kChunkDim + z;
      if (heightmap_block_change_[idx] == INT8_MIN) {
        continue;  // Nothing is changed
      }

      int max_y = heightmap_block_change_[idx];
      heightmap_block_change_[idx] = INT8_MIN;

      // Cast a shadow
      int y = max_y;
      int max_propagation =
          TryPropagateSkylight(tasks.GetChunkPos(), BlockPos{x, y, z});

      y = max_propagation;
      ChunkPos curr_chunk_pos = tasks.GetChunkPos();
      curr_chunk_pos.y = y >> kChunkDimLog2;
      TryPropagateShadow(curr_chunk_pos, BlockPos{x, y & (kChunkDim - 1), z});
    }
  }

  DelayIncrease();
  DelayDecrease();

  // Then check blocks now
  for (const auto& task : tasks.GetLightTask()) {
    CheckBlock(tasks.GetChunkPos(), task.GetLocalPos());
  }

  PropagateDecrease();
}

void SkyLightEngine::DelayDecrease() {
  for (size_t i = 0; i < enqueue_decrease_pos_; ++i) {
    InternalTask& task = decrease_queue_[i];
    SetLightLvl(task.GetChunkPos().GetBlockPosOffset() + task.GetBlockPos(), 0);
  }
}

void SkyLightEngine::DelayIncrease() {
  for (size_t i = 0; i < enqueue_increase_pos_; ++i) {
    InternalTask& task = increase_queue_[i];
    SetLightLvl(task.GetChunkPos().GetBlockPosOffset() + task.GetBlockPos(),
                task.GetLightLevel());
  }
}

int SkyLightEngine::TryPropagateSkylight(ChunkPos chunk_pos,
                                         BlockPos block_pos) {
  if (!CheckChunk(chunk_pos)) {
    return block_pos.y;
  }

  int x = chunk_pos.x * kChunkDim + block_pos.x;
  int y = chunk_pos.y * kChunkDim + block_pos.y;
  int z = chunk_pos.z * kChunkDim + block_pos.z;

  // Check if the top of the column is covered or theres no chunk above
  int y_top = y + 1;
  if (CheckChunk(BlockPos{x, y_top, z}.ToChunkPos()) &&
      GetLightLvl(BlockPos{x, y_top, z}) != kMaxLightLevel) {
    return y;
  }

  ChunkPos curr_chunk_pos = chunk_pos;
  Chunk* curr_chunk = GetChunk(chunk_pos);
  int curr_opacity = 0;
  BlockID curr_block;

  // Terminates if it hits an opaque block or bottom of world
  while (true) {
    curr_block = GetBlock(BlockPos{x, y, z});
    curr_opacity =
        game_context_.blocks_->GetBlockProperties(curr_block).opacity_;

    if (curr_opacity > 0) {
      break;
    }

    InternalTask task;
    task.SetBlockPos(BlockPos{x, y, z}.GetLocalPos());
    task.SetChunkPos(curr_chunk_pos);
    task.SetDirection(kAllDirections);
    task.SetLightLevel(kMaxLightLevel);
    EnqueueIncrease(task);

    --y;
    if ((y & (kChunkDim - 1)) == (kChunkDim - 1)) {
      --curr_chunk_pos.y;
      if (!CheckChunk(curr_chunk_pos)) {
        return y + 1;
      }
      curr_chunk = GetChunk(curr_chunk_pos);
    }
  }

  return y;
}

void SkyLightEngine::TryPropagateShadow(ChunkPos chunk_pos,
                                        BlockPos block_pos) {
  if (!CheckChunk(chunk_pos)) {
    return;
  }

  int x = chunk_pos.x * kChunkDim + block_pos.x;
  int y = chunk_pos.y * kChunkDim + block_pos.y;
  int z = chunk_pos.z * kChunkDim + block_pos.z;

  // Check if the column is already in a shadow
  int y_top = y + 1;
  if (CheckChunk(BlockPos{x, y_top, z}.ToChunkPos()) &&
      GetLightLvl(BlockPos{x, y_top, z}) == 0) {
    return;
  }

  Chunk* curr_chunk = GetChunk(chunk_pos);
  int curr_opacity = 0;
  BlockID curr_block;
  int curr_light_lvl;
  // Terminate if it goes out of world or hit block with an opacity > 0
  while (true) {
    curr_light_lvl = GetLightLvl(BlockPos{x, y, z});
    if (curr_light_lvl == 0) break;
    InternalTask task;
    task.SetBlockPos(BlockPos{x, y, z}.GetLocalPos());
    task.SetChunkPos(chunk_pos);
    task.SetDirection(kAllDirections);
    task.SetLightLevel(curr_light_lvl);
    EnqueueDecrease(task);

    --y;
    if ((y & (kChunkDim - 1)) == (kChunkDim - 1)) {  // Enters new chunk
      --chunk_pos.y;
      if (!CheckChunk(chunk_pos)) break;
      curr_chunk = GetChunk(chunk_pos);
    }
  }
}
