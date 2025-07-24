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
        task.SetBlockPos(BlockPos{x, y, z} + chunk_pos.GetBlockPosOffset());
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

  for (auto direction : Directions<ChunkPos>()) {
    if (direction == Directions<ChunkPos>::kDown) {
      continue;
    }

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

    BlockPos curr_pos;

    for (int u = 0; u < kChunkDim; ++u) {
      for (int v = 0; v < kChunkDim; ++v) {
        curr_pos = curr_chunk_pos.GetBlockPosOffset();
        curr_pos[axis] = direction.IsNegative() * (kChunkDim - 1);
        curr_pos[axis_u] = u;
        curr_pos[axis_v] = v;

        int curr_light = GetLightLvl(curr_pos);

        if (curr_light <= 1) {
          continue;
        }

        BlockPos next_pos = curr_pos;
        next_pos.IncrementSide(direction.GetOppositeDirection(), 1);
        BlockID neigh_block = GetBlock(next_pos);

        int opacity =
            game_context_.blocks_->GetBlockProperties(neigh_block).opacity_;
        int spread_lvl = curr_light - std::max(1, opacity);
        if (spread_lvl <= 0) {
          continue;
        }

        InternalTask task;
        task.SetBlockPos(next_pos);
        task.SetLightLevel(spread_lvl);
        task.SetDirection(direction);
        EnqueueIncrease(task);
      }
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
      BlockPos pos = tasks.GetChunkPos().GetBlockPosOffset();
      pos += BlockPos{x, y, z};
      int max_propagation = TryPropagateSkylight(pos);
      pos.y = max_propagation;

      TryPropagateShadow(pos);
    }
  }

  DelayIncrease();
  DelayDecrease();

  // Then check blocks now
  for (const auto& task : tasks.GetLightTask()) {
    CheckBlock(task.GetLocalPos() + tasks.GetChunkPos().GetBlockPosOffset());
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
  if (!CheckChunk(block_pos.ToChunkPos())) {
    return block_pos.y;
  }

  BlockPos block_top_pos = block_pos;
  ++block_top_pos.y;

  // Check if the top of the column is covered or theres no chunk above
  if (CheckChunk(block_top_pos.ToChunkPos()) &&
      GetLightLvl(block_top_pos) != kMaxLightLevel) {
    return block_pos.y;
  }

  Chunk* curr_chunk = GetChunk(block_pos.ToChunkPos());
  int curr_opacity = 0;
  BlockID curr_block;

  int local_x = block_pos.x & (kChunkDim - 1);
  int local_z = block_pos.z & (kChunkDim - 1);

  // Terminates if it hits an opaque block or bottom of world
  while (true) {
    // Use heightmap to optimize finding opaque block
    int height = curr_chunk->heightmap_->Get(local_x, local_z);
    int bot_y = block_pos.ToChunkPos().y * kChunkDim;
    for (; block_pos.y > bot_y + height; --block_pos.y) {
      InternalTask task;
      task.SetBlockPos(block_pos);
      task.SetDirection(kAllDirections);
      task.SetLightLevel(kMaxLightLevel);
      EnqueueIncrease(task);
    }

    if (height != -1) {
      break;
    }

    if (!CheckChunk(block_pos.ToChunkPos())) {
      return block_pos.y;
    }
  }

  return block_pos.y;
}

void SkyLightEngine::TryPropagateShadow(BlockPos block_pos) {
  if (!CheckChunk(block_pos.ToChunkPos())) {
    return;
  }

  // Check if the column is already in a shadow
  BlockPos block_top_pos = block_pos;
  ++block_top_pos.y;
  if (CheckChunk(block_top_pos.ToChunkPos()) &&
      GetLightLvl(block_top_pos) == 0) {
    return;
  }

  Chunk* curr_chunk = GetChunk(block_pos.ToChunkPos());
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
      if (!CheckChunk(block_pos.ToChunkPos())) break;
      curr_chunk = GetChunk(block_pos.ToChunkPos());
    }
  }
}
