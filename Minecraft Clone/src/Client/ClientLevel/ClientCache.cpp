// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientLevel/ClientCache.h"

#include <memory>
#include <utility>

#include "Client/ClientLevel/Entity/ClientEntities.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Container/Region.h"
#include "Level/World/CollusionDetector.h"

ClientCache::ClientCache(GameContext& context)
    : context_{context},
      entities_{context},
      collusion_manager_{context, &chunk_cache_},
      chunk_cache_{context} {}
ClientCache::~ClientCache() = default;

void ClientCache::AddChunk(std::unique_ptr<Chunk> chunk) {
  chunk_cache_.InsertChunk(std::move(chunk));
}

void ClientCache::EraseChunk(ChunkPos pos) { chunk_cache_.EraseChunk(pos); }

Chunk* ClientCache::GetChunk(ChunkPos pos) const {
  return chunk_cache_.GetChunk(pos);
}

BlockID ClientCache::GetBlock(BlockPos pos) const {
  return chunk_cache_.GetBlock(pos);
}

bool ClientCache::CheckChunk(ChunkPos pos) const {
  return chunk_cache_.CheckChunk(pos);
}

void ClientCache::InsertEntity(const EntityProperty& entity) {
  entities_.InsertEntity(entity);
}

void ClientCache::UpdateEntity(const EntityProperty& entity) {
  entities_.UpdateEntity(entity);
}

void ClientCache::RemoveEntity(const EntityUUID& uuid) {
  entities_.RemoveEntity(uuid);
}
