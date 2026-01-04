// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientLevel/ClientCache.h"

#include <memory>
#include <utility>

#include "Client/ClientLevel/Entity/ClientEntities.h"
#include "Client/ECS/ClientECSManager.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Container/Region.h"
#include "Level/Entity/Properties/EntityProperties.h"
#include "Level/World/CollisionDetector.h"

ClientCache::ClientCache(GameContext& context)
    : WorldInterface{context, false, false},
      context_{context},
      entities_{context},
      collision_manager_{context, *chunks_} {
  // Create client-side ECS manager with minimal systems
  ecs_manager_ = std::make_unique<ClientECSManager>(context, *this);
}

ClientCache::~ClientCache() = default;

void ClientCache::AddChunk(std::unique_ptr<Chunk> chunk) {
  chunks_->InsertChunk(std::move(chunk));
}

void ClientCache::EraseChunk(ChunkPos pos) { chunks_->EraseChunk(pos); }

void ClientCache::InsertEntity(const EntityProperty& entity) {
  entities_.InsertEntity(entity);
}

void ClientCache::UpdateEntity(const EntityProperty& entity) {
  entities_.UpdateEntity(entity);
}

void ClientCache::RemoveEntity(const EntityUUID& uuid) {
  entities_.RemoveEntity(uuid);
}
