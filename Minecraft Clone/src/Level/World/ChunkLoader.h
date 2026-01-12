#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <deque>

#include "Core/Typenames.h"
#include "Level/World/WorldParameters.h"

class World;
class Chunk;

/*
Purpose: Handles logic for determining which chunks need to be loaded based on
spawn chunks and entity loaders.
*/
class ChunkLoader {
 public:
  ChunkLoader(World* world, WorldParameters settings);

  void Load();

  void DeleteEntityChunkLoader(EntityUUID uuid);
  bool CheckEntityExistChunkLoader(EntityUUID uuid) const;
  void SetEntityChunkLoader(EntityUUID uuid);
  void ClearRequests();
  
  // Getters
  const std::vector<ChunkPos>& GetRequestedChunks() const;
  
  // Public for configuration if needed
  bool tall_generation_ = false;

 private:
  bool RequestLoad(ChunkPos pos);
  void LoadSpawnChunks();
  void LoadSpawnEntitySurrounding(EntityUUID uuid);
  void LoadMovingEntitySurrounding();

  World* world_ = nullptr;
  std::unique_ptr<WorldParameters> settings_;

  FastHashSet<ChunkPos> generating_chunk_;
  bool is_spawn_chunks_loaded_ = false;

  FastHashSet<EntityUUID> entity_chunk_loaders_;
  FastHashMap<EntityUUID, ChunkPos> entity_last_chunk_;
  std::vector<EntityUUID> chunk_loader_queue_;

  // Output state
  std::vector<ChunkPos> chunk_request_;
};
