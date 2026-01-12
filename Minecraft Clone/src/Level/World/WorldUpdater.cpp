#include "Level/World/WorldUpdater.h"

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <deque>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int3.hpp>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Container/EntityContainer.h"
#include "Level/Entity/Entity.h"
#include "Level/World/World.h"
#include "Level/World/WorldParameters.h"
#include "Utils/Assert.h"
#include "Utils/Iterators.h"

using std::make_unique;
using std::move;
using std::floor;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;

WorldUpdater::WorldUpdater(GameContext& context, World* w, WorldParameters p)
    : context_{context},
      settings_{make_unique<WorldParameters>(p)},
      world_{w} {}

void WorldUpdater::loadSummonEntitySurrounding(EntityUUID uuid) {
  Entity* e = world_->GetEntity(uuid);
  GAME_ASSERT(e != nullptr, "Entity with UUID " + to_string(uuid) + " not found");

  // Calculate entity chunk position
  int chunk_x = static_cast<int>(std::floor(e->properties_.position_.x / kChunkDim));
  int chunk_y = static_cast<int>(std::floor(e->properties_.position_.y / kChunkDim));
  int chunk_z = static_cast<int>(std::floor(e->properties_.position_.z / kChunkDim));
  ChunkPos entity_chunk{chunk_x, chunk_y, chunk_z};

  // Store entity's initial chunk position for future tracking
  entity_last_chunk_[uuid] = entity_chunk;

  glm::ivec3 initial_pos{chunk_x, chunk_y, chunk_z};

  // BFS with visited set to prevent exponential redundant processing
  std::deque<glm::ivec3> fifo;
  FastHashSet<ChunkPos> visited;

  fifo.push_back(initial_pos);
  visited.insert(entity_chunk);

  while (!fifo.empty()) {
    glm::ivec3 current_chunk_pos = fifo.front();
    fifo.pop_front();

    // Gets relative position from the entity
    glm::ivec3 offset = initial_pos - current_chunk_pos;

    // Checks if it is in range
    if ((abs(offset.x) > settings_->horizontal_ticking_distance_) ||
        (abs(offset.z) > settings_->horizontal_ticking_distance_))
      continue;
    if (abs(offset.y) > settings_->vertical_ticking_distance_) continue;

    bool isSuccess = RequestLoad(
        {current_chunk_pos.x, current_chunk_pos.y, current_chunk_pos.z});
    if (!isSuccess) continue;

    for (int side = 0; side < 6; side++) {
      glm::ivec3 neighbor_chunk_pos = current_chunk_pos;
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


bool WorldUpdater::RequestLoad(ChunkPos pos) {
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

// Only work on loading chunks for now. Unloading for later
void WorldUpdater::loadSurroundedMovedEntityChunk() {
  glm::ivec3 axis_tick_dist{settings_->horizontal_ticking_distance_,
                            settings_->vertical_ticking_distance_,
                            settings_->horizontal_ticking_distance_};

  for (const auto& uuid : entity_chunk_loaders_) {
    Entity* entity = world_->GetEntity(uuid);

    GAME_ASSERT(entity != nullptr,
                string("Entity with UUID " + to_string(uuid) + " not found"));

    // Calculate current chunk position
    int x = static_cast<int>(floor(entity->properties_.position_.x / kChunkDim));
    int y = static_cast<int>(floor(entity->properties_.position_.y / kChunkDim));
    int z = static_cast<int>(floor(entity->properties_.position_.z / kChunkDim));
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
    glm::vec3 vel = entity->properties_.velocity_;
    if (vel.x == 0.f && vel.y == 0.f && vel.z == 0.f) {
      continue;
    }

    constexpr int chunk_padding = 4;
    glm::ivec3 pos{x, y, z};

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
        glm::ivec3 test_position = pos;

        test_position[(j + 1) % 3] += u;
        test_position[(j + 2) % 3] += v;
        test_position[j] += sideDistanceOffset * side;

        RequestLoad({test_position.x, test_position.y, test_position.z});
      }
    }
  }
}


void WorldUpdater::loadSpawnChunks() {
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

void WorldUpdater::DeleteEntityChunkLoader(EntityUUID uuid) {
  if (!entity_chunk_loaders_.count(uuid))
    throw std::logic_error(
        "WorldLoader::DeleteEntityChunkLoader - " +
        string("Could not find entity with UUID " + to_string(uuid)));
  entity_chunk_loaders_.erase(uuid);
  entity_last_chunk_.erase(uuid);  // Clean up tracking
}

bool WorldUpdater::CheckEntityExistChunkLoader(EntityUUID uuid) const {
  return entity_chunk_loaders_.count(uuid);
}

void WorldUpdater::Load() {
  if (!is_spawn_chunks_loaded_) loadSpawnChunks();

  for (EntityUUID uuid : chunk_loader_queue_) {
    loadSummonEntitySurrounding(uuid);
  }
  chunk_loader_queue_.clear();

  loadSurroundedMovedEntityChunk();
}

// Getters

vector<ChunkPos> WorldUpdater::GetLightUpdate() {
  auto chunks = world_->GetAllChunks();
  vector<ChunkPos> out;

  for (auto& chunk : chunks) {
    if (chunk->CheckLightDirty()) {
      out.emplace_back(chunk->position_);
    }
  }

  return out;
}

const vector<ChunkPos>& WorldUpdater::GetCreatedChunkPos() {
  return created_chunk_arr_;
}

const vector<ChunkPos>& WorldUpdater::GetRequestedChunks() {
  return chunk_request_;
}

vector<EntityProperty> WorldUpdater::GetSpawnedEntities() {
  return world_->GetEntityContainer()->GetSpawnedEntities();
}

vector<EntityProperty> WorldUpdater::GetUpdatedEntities() {
  return world_->GetEntityContainer()->GetUpdatedEntities();
}

vector<EntityUUID> WorldUpdater::GetRemovedEntities() {
  return world_->GetEntityContainer()->GetRemovedEntities();
}

const FastHashMap<ChunkPos, vector<BlockPos>>&
WorldUpdater::GetChangedBlocks() {
  return changed_block_;
}

// Setters
void WorldUpdater::SetBlock(const BlockID& block, BlockPos pos) {
  world_->SetBlock(block, pos);

  auto it = changed_block_.find(pos.ToChunkPos());
  if (it == changed_block_.end()) {
    auto res = changed_block_.emplace(pos.ToChunkPos(), vector<BlockPos>{});
    if (!res.second) {
      throw std::runtime_error(
          "WorldUpdater::SetBlock - Failed to updated changed block list");
    }
    it = res.first;
  }
  it->second.emplace_back(pos.GetLocalPos());
}

void WorldUpdater::SetEntityChunkLoader(EntityUUID uuid) {
  entity_chunk_loaders_.insert(uuid);
  chunk_loader_queue_.push_back(uuid);
}

void WorldUpdater::SetChunk(unique_ptr<Chunk> chunk) {
  vector<unique_ptr<Chunk>> arr;
  arr.push_back(move(chunk));
  SetChunk(move(arr));
}

void WorldUpdater::SetChunk(vector<unique_ptr<Chunk>>&& chunks) {
  vector<ChunkPos> updated_pos;
  updated_pos.reserve(chunks.size());

  for (auto& chunk : chunks) {
    updated_pos.push_back(chunk->position_);
    world_->SetChunk(move(chunk));
  }

  for (auto pos : updated_pos) {
    if (created_chunk_.insert(pos).second) {
      created_chunk_arr_.emplace_back(pos);

      for (auto offset : Directions<ChunkPos>()) {
        ChunkPos neighbor_pos = pos + offset;

        if (world_->CheckChunk(neighbor_pos) &&
            !created_chunk_.contains(neighbor_pos)) {
          created_chunk_arr_.push_back(neighbor_pos);
          created_chunk_.insert(neighbor_pos);
        }
      }
    }
  }
}

EntityUUID WorldUpdater::SetEntity(unique_ptr<Entity> entity) {
  bool is_chunk_loader = entity->properties_.is_chunk_loader_;
  EntityUUID uuid = world_->SetEntity(move(entity));
  if (is_chunk_loader) {
    SetEntityChunkLoader(uuid);
  }
  return uuid;
}

void WorldUpdater::KillEntity(const EntityUUID& uuid) {
  world_->RemoveEntity(uuid);
  if (CheckEntityExistChunkLoader(uuid)) {
    DeleteEntityChunkLoader(uuid);
  }
}

void WorldUpdater::ResetState() {
  created_chunk_.clear();
  created_chunk_arr_.clear();
  changed_block_.clear();
  chunk_request_.clear();

  world_->GetEntityContainer()->ResetState();
}
