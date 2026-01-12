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
      world_{w},
      chunk_loader_{make_unique<ChunkLoader>(w, p)} {}

void WorldUpdater::SetTallGeneration(bool val) {
  chunk_loader_->tall_generation_ = val;
}

bool WorldUpdater::IsTallGeneration() const {
  return chunk_loader_->tall_generation_;
}

void WorldUpdater::DeleteEntityChunkLoader(EntityUUID uuid) {
  chunk_loader_->DeleteEntityChunkLoader(uuid);
}

bool WorldUpdater::CheckEntityExistChunkLoader(EntityUUID uuid) const {
  return chunk_loader_->CheckEntityExistChunkLoader(uuid);
}

void WorldUpdater::Load() {
  chunk_loader_->Load();
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
  return chunk_loader_->GetRequestedChunks();
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
  chunk_loader_->SetEntityChunkLoader(uuid);
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
  
  chunk_loader_->ClearRequests();
  
  world_->GetEntityContainer()->ResetState();
}
