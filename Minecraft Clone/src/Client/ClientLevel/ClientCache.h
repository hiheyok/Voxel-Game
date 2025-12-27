// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>
#include <utility>

#include "Client/ClientLevel/Entity/ClientEntities.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Container/Region.h"
#include "Level/World/CollusionDetector.h"
#include "Level/World/WorldInterface.h"

class GameContext;

class ClientCache : public WorldInterface {
 public:
  ClientCache(GameContext&);
  ~ClientCache();
  
  void AddChunk(std::unique_ptr<Chunk> chunk);
  void EraseChunk(ChunkPos pos);
  
  void InsertEntity(const EntityProperty& entity);
  void UpdateEntity(const EntityProperty& entity);
  void RemoveEntity(const EntityUUID& uuid);

  GameContext& context_;
  ClientEntities entities_;
  CollusionDetector collusion_manager_;
};
