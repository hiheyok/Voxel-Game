// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>
#include <utility>

#include "Client/ClientLevel/Entity/ClientEntities.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Container/Region.h"
#include "Level/World/CollusionDetector.h"

class GameContext;

class ClientCache {
 public:
  ClientCache(GameContext&);
  ~ClientCache();
  
  void AddChunk(std::unique_ptr<Chunk> chunk);
  void EraseChunk(ChunkPos pos);
  Chunk* GetChunk(ChunkPos pos) const;
  BlockID GetBlock(BlockPos pos) const;
  bool CheckChunk(ChunkPos pos) const;
  void InsertEntity(const EntityProperty& entity);
  void UpdateEntity(const EntityProperty& entity);
  void RemoveEntity(const EntityUUID& uuid);

  GameContext& game_context_;
  ClientEntities entities_;
  CollusionDetector collusion_manager_;

 private:
  ChunkMap chunk_cache_;
};
