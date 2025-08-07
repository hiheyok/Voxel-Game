#include "Level/World/WorldUpdater.h"

#include <deque>
#include <glm/vec3.hpp>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Container/EntityContainer.h"
#include "Level/Entity/Entity.h"
#include "Level/Light/LightStorage.h"
#include "Level/World/World.h"
#include "Level/World/WorldParameters.h"

WorldUpdater::WorldUpdater(GameContext& context, World* w, WorldParameters p)
    : context_{context},
      settings_{std::make_unique<WorldParameters>(p)},
      world_{w} {}

void WorldUpdater::loadSummonEntitySurrounding(EntityUUID uuid) {
  Entity* e = world_->GetEntity(uuid);

  if (e == nullptr) {
    throw std::logic_error(
        "WorldLoader::loadSummonEntitySurrounding - " +
        std::string("Entity with UUID " + std::to_string(uuid) + " not found"));
    return;
  }
  glm::vec3 pos = e->properties_.position_ / 16.f;
  // vec3 velocity = e->Properties.Velocity;

  glm::ivec3 initial_pos{pos.x, pos.y, pos.z};

  // TODO(hiheyok): Use light engine FIFO
  std::deque<glm::ivec3> fifo;

  fifo.push_back(initial_pos);
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

    bool isSuccess = RequestLoad(ChunkPos{
        current_chunk_pos.x, current_chunk_pos.y, current_chunk_pos.z});
    if (!isSuccess) continue;

    for (int side = 0; side < 6; side++) {
      glm::ivec3 neighbor_chunk_pos = current_chunk_pos;

      neighbor_chunk_pos[side >> 1] += (side & 0b1) * 2 - 1;

      fifo.push_back(neighbor_chunk_pos);
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

// Only work on loading chunks for not. Unloading for later
void WorldUpdater::loadSurroundedMovedEntityChunk() {
  std::vector<glm::ivec3> center_position_list;
  std::vector<glm::vec3> center_velocity_list;

  // Get entity chunk position

  for (const auto& e : entity_chunk_loaders_) {
    Entity* entity = world_->GetEntity(e);

    if (entity == nullptr)
      throw std::logic_error(
          "WorldLoader::loadSurroundedMovedEntityChunk - " +
          std::string("Entity with UUID " + std::to_string(e) + " not found"));

    int x = static_cast<int>(entity->properties_.position_.x / kChunkDim);
    int y = static_cast<int>(entity->properties_.position_.y / kChunkDim);
    int z = static_cast<int>(entity->properties_.position_.z / kChunkDim);

    if ((entity->properties_.velocity_.x == 0.f) &&
        (entity->properties_.velocity_.y == 0.f) &&
        (entity->properties_.velocity_.z == 0.f)) {
      continue;
    }

    center_position_list.emplace_back(x, y, z);
    center_velocity_list.emplace_back(entity->properties_.velocity_);
  }

  if (center_position_list.empty()) return;

  glm::ivec3 axis_tick_dist{settings_->horizontal_ticking_distance_,
                            settings_->vertical_ticking_distance_,
                            settings_->horizontal_ticking_distance_};

  int chunk_padding = 4;

  for (size_t i = 0; i < center_position_list.size(); i++) {
    glm::ivec3 pos = center_position_list[i];
    glm::vec3 vel = center_velocity_list[i];

    for (int j = 0; j < 3; j++) {
      int side = 0;

      if (vel[j] > 0) {
        side = 1;
      }

      if (vel[j] < 0) {
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

        // Test if it exist of generating
        bool isSuccess = RequestLoad(
            ChunkPos{test_position.x, test_position.y, test_position.z});
        if (!isSuccess) continue;
      }
    }
  }
}

#include <ranges>

void WorldUpdater::loadSpawnChunks() {
  assert(world_ != nullptr);

  int x0 = -settings_->spawn_chunk_horizontal_radius_;
  int x1 = settings_->spawn_chunk_horizontal_radius_ + 1;
  int y0 = -settings_->spawn_chunk_vertical_radius_;
  int y1 = settings_->spawn_chunk_vertical_radius_ + 1;
  int z0 = -settings_->spawn_chunk_horizontal_radius_;
  int z1 = settings_->spawn_chunk_horizontal_radius_ + 1;

  // Generate chunk in reverse as a temp solution to lighting bug
  auto it = Product<3>({{x0, x1}, {y0, y1}, {z0, z1}});

  for (const auto& [x, y, z] : it | std::views::reverse) {
    if (!RequestLoad(ChunkPos{x, y, z})) {
      continue;
    }
  }

  is_spawn_chunks_loaded_ = true;
}

void WorldUpdater::DeleteEntityChunkLoader(EntityUUID uuid) {
  if (!entity_chunk_loaders_.count(uuid))
    throw std::logic_error(
        "WorldLoader::DeleteEntityChunkLoader - " +
        std::string("Could not find entity with UUID " + std::to_string(uuid)));
  entity_chunk_loaders_.erase(uuid);
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

std::vector<ChunkPos> WorldUpdater::GetLightUpdate() {
  auto chunks = world_->GetAllChunks();
  std::vector<ChunkPos> out;

  for (auto& chunk : chunks) {
    if (chunk->CheckLightDirty()) {
      out.emplace_back(chunk->position_);
    }
  }

  return out;
}

const std::vector<ChunkPos>& WorldUpdater::GetCreatedChunkPos() {
  return created_chunk_arr_;
}

const std::vector<ChunkPos>& WorldUpdater::GetRequestedChunks() {
  return chunk_request_;
}

std::vector<EntityProperty> WorldUpdater::GetSpawnedEntities() {
  return world_->GetEntityContainer()->GetSpawnedEntities();
}

std::vector<EntityProperty> WorldUpdater::GetUpdatedEntities() {
  return world_->GetEntityContainer()->GetUpdatedEntities();
}

std::vector<EntityUUID> WorldUpdater::GetRemovedEntities() {
  return world_->GetEntityContainer()->GetRemovedEntities();
}

const FastHashMap<ChunkPos, std::vector<BlockPos>>&
WorldUpdater::GetChangedBlocks() {
  return changed_block_;
}

// Setters
void WorldUpdater::SetBlock(const BlockID& block, BlockPos pos) {
  world_->SetBlock(block, pos);

  auto it = changed_block_.find(pos.ToChunkPos());
  if (it == changed_block_.end()) {
    auto res =
        changed_block_.emplace(pos.ToChunkPos(), std::vector<BlockPos>{});
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

void WorldUpdater::SetChunk(std::unique_ptr<Chunk> chunk) {
  std::vector<std::unique_ptr<Chunk>> arr;
  arr.push_back(std::move(chunk));
  SetChunk(std::move(arr));
}

void WorldUpdater::SetChunk(std::vector<std::unique_ptr<Chunk>> chunks) {
  std::vector<ChunkPos> updated_pos;
  updated_pos.reserve(chunks.size());

  for (auto& chunk : chunks) {
    updated_pos.push_back(chunk->position_);
    world_->SetChunk(std::move(chunk));
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

EntityUUID WorldUpdater::SetEntity(std::unique_ptr<Entity> entity) {
  bool is_chunk_loader = entity->properties_.is_chunk_loader_;
  EntityUUID uuid = world_->SetEntity(std::move(entity));
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
