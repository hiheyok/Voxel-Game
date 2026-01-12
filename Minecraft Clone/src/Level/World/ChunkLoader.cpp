#include "Level/World/ChunkLoader.h"

#include <cmath>
#include <deque>
#include <ranges>
#include <stdexcept>
#include <string>
#include <utility>

#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Entity/Entity.h"
#include "Level/World/World.h"
#include "Utils/Assert.h"
#include "Utils/Iterators.h"

using glm::ivec3;
using glm::vec3;
using std::deque;
using std::floor;
using std::make_unique;
using std::string;
using std::to_string;
using std::vector;

ChunkLoader::ChunkLoader(World* world, WorldParameters settings)
    : world_{world}, settings_{make_unique<WorldParameters>(settings)} {}

void ChunkLoader::Load() {
  if (!is_spawn_chunks_loaded_) LoadSpawnChunks();

  for (EntityUUID uuid : chunk_loader_queue_) {
    LoadSpawnEntitySurrounding(uuid);
  }
  chunk_loader_queue_.clear();

  LoadMovingEntitySurrounding();
}

bool ChunkLoader::RequestLoad(ChunkPos pos) {
  ChunkPos p = pos;
  if (world_->CheckChunk(p)) return false;

  if (tall_generation_) p.y /= 16;

  if (generating_chunk_.insert(p).second) {
    chunk_request_.emplace_back(p);
    return true;
  } else {
    return false;
  }
}

void ChunkLoader::LoadSpawnChunks() {
  GAME_ASSERT(world_ != nullptr, "World is null");

  int x0 = -settings_->spawn_chunk_horizontal_radius_;
  int x1 = settings_->spawn_chunk_horizontal_radius_ + 1;
  int y0 = -settings_->spawn_chunk_vertical_radius_;
  int y1 = settings_->spawn_chunk_vertical_radius_ + 1;
  int z0 = -settings_->spawn_chunk_horizontal_radius_;
  int z1 = settings_->spawn_chunk_horizontal_radius_ + 1;

  // Generate chunk in reverse as a temp solution to lighting bug
  auto it = Product<3>({{x0, x1}, {y0, y1}, {z0, z1}});

  for (const auto& [x, y, z] : it | std::views::reverse) {
    if (!RequestLoad({x, y, z})) {
      continue;
    }
  }

  is_spawn_chunks_loaded_ = true;
}

void ChunkLoader::LoadSpawnEntitySurrounding(EntityUUID uuid) {
  Entity* e = world_->GetEntity(uuid);
  GAME_ASSERT(e != nullptr,
              "Entity with UUID " + to_string(uuid) + " not found");

  // Calculate entity chunk position
  int chunk_x = static_cast<int>(floor(e->properties_.position_.x / kChunkDim));
  int chunk_y = static_cast<int>(floor(e->properties_.position_.y / kChunkDim));
  int chunk_z = static_cast<int>(floor(e->properties_.position_.z / kChunkDim));
  ChunkPos entity_chunk{chunk_x, chunk_y, chunk_z};

  // Store entity's initial chunk position for future tracking
  entity_last_chunk_[uuid] = entity_chunk;

  ivec3 initial_pos{chunk_x, chunk_y, chunk_z};

  // BFS with visited set to prevent exponential redundant processing
  deque<ivec3> fifo;
  FastHashSet<ChunkPos> visited;

  fifo.push_back(initial_pos);
  visited.insert(entity_chunk);

  while (!fifo.empty()) {
    ivec3 current_chunk_pos = fifo.front();
    fifo.pop_front();

    // Gets relative position from the entity
    ivec3 offset = initial_pos - current_chunk_pos;

    // Checks if it is in range
    if (abs(offset.x) > settings_->horizontal_ticking_distance_ ||
        abs(offset.z) > settings_->horizontal_ticking_distance_ ||
        abs(offset.y) > settings_->vertical_ticking_distance_) {
      continue;
    }

    bool success = RequestLoad(
        {current_chunk_pos.x, current_chunk_pos.y, current_chunk_pos.z});
    if (!success) continue;

    for (int side = 0; side < 6; side++) {
      ivec3 neighbor_chunk_pos = current_chunk_pos;
      neighbor_chunk_pos[side >> 1] += (side & 0b1) * 2 - 1;

      ChunkPos neighbor{neighbor_chunk_pos.x, neighbor_chunk_pos.y,
                        neighbor_chunk_pos.z};
      // Only add if not visited
      if (visited.insert(neighbor).second) {
        fifo.push_back(neighbor_chunk_pos);
      }
    }
  }
}

void ChunkLoader::LoadMovingEntitySurrounding() {
  ivec3 axis_tick_dist{settings_->horizontal_ticking_distance_,
                       settings_->vertical_ticking_distance_,
                       settings_->horizontal_ticking_distance_};

  for (const auto& uuid : entity_chunk_loaders_) {
    Entity* entity = world_->GetEntity(uuid);

    GAME_ASSERT(entity != nullptr, "Entity is null");

    // Calculate current chunk position
    vec3 entity_pos = entity->properties_.position_;
    int x = static_cast<int>(floor(entity_pos.x / kChunkDim));
    int y = static_cast<int>(floor(entity_pos.y / kChunkDim));
    int z = static_cast<int>(floor(entity_pos.z / kChunkDim));
    ChunkPos current_chunk{x, y, z};

    // Check if entity changed chunks
    auto it = entity_last_chunk_.find(uuid);
    if (it != entity_last_chunk_.end() && it->second != current_chunk) {
      // Entity moved to a new chunk - load all surrounding chunks
      for (auto [dx, dy, dz] :
           Product<3>({{-axis_tick_dist.x, axis_tick_dist.x + 1},
                       {-axis_tick_dist.y, axis_tick_dist.y + 1},
                       {-axis_tick_dist.z, axis_tick_dist.z + 1}})) {
        RequestLoad({x + dx, y + dy, z + dz});
      }
      // Update last known chunk
      it->second = current_chunk;
    }

    // Velocity-based directional loading for moving entities
    vec3 vel = entity->properties_.velocity_;
    if (vel.x == 0.0f && vel.y == 0.0f && vel.z == 0.0f) {
      continue;
    }

    constexpr int chunk_padding = 4;
    ivec3 pos{x, y, z};

    for (int j = 0; j < 3; j++) {
      int side = 0;
      if (vel[j] > 0) {
        side = 1;
      } else if (vel[j] < 0) {
        side = -1;
      }

      if (side == 0) continue;

      int u_d = axis_tick_dist[(j + 1) % 3] + chunk_padding;
      int v_d = axis_tick_dist[(j + 2) % 3] + chunk_padding;
      int sideDistanceOffset = axis_tick_dist[j] + 1;

      for (auto [u, v] : Product<2>({{-u_d, u_d + 1}, {-v_d, v_d + 1}})) {
        ivec3 test_position = pos;

        test_position[(j + 1) % 3] += u;
        test_position[(j + 2) % 3] += v;
        test_position[j] += sideDistanceOffset * side;

        RequestLoad({test_position.x, test_position.y, test_position.z});
      }
    }
  }
}

void ChunkLoader::DeleteEntityChunkLoader(EntityUUID uuid) {
  GAME_ASSERT(entity_chunk_loaders_.count(uuid), "Couldn't find entity.");
  entity_chunk_loaders_.erase(uuid);
  entity_last_chunk_.erase(uuid);  // Clean up tracking
}

bool ChunkLoader::CheckEntityExistChunkLoader(EntityUUID uuid) const {
  return entity_chunk_loaders_.count(uuid);
}

void ChunkLoader::SetEntityChunkLoader(EntityUUID uuid) {
  entity_chunk_loaders_.insert(uuid);
  chunk_loader_queue_.push_back(uuid);
}

void ChunkLoader::ClearRequests() { chunk_request_.clear(); }

const vector<ChunkPos>& ChunkLoader::GetRequestedChunks() const {
  return chunk_request_;
}
