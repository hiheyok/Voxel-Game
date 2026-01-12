#pragma once
#include <memory>
#include <mutex>
#include <tuple>
#include <vector>

#include "Core/Typenames.h"

class World;
class Chunk;
class LightStorage;
class GameContext;

struct Entity;
struct EntityProperty;
struct WorldParameters;

// Add chunk unloading later when entity goes out of range
/*
Purpose: Handles updating the world or dimension and communication about world
updates
*/
class WorldUpdater {
 public:
  bool tall_generation_ = false;

  WorldUpdater(GameContext&, World* w, WorldParameters p);

  void DeleteEntityChunkLoader(EntityUUID uuid);
  bool CheckEntityExistChunkLoader(EntityUUID uuid) const;
  void Load();

  // Getters
  std::vector<ChunkPos> GetLightUpdate();
  const std::vector<ChunkPos>& GetCreatedChunkPos();
  const std::vector<ChunkPos>& GetRequestedChunks();
  std::vector<EntityProperty> GetUpdatedEntities();
  std::vector<EntityProperty> GetSpawnedEntities();
  std::vector<EntityUUID> GetRemovedEntities();
  const FastHashMap<ChunkPos, std::vector<BlockPos>>& GetChangedBlocks();

  // Setters
  void SetChunk(std::unique_ptr<Chunk> chunk);
  void SetChunk(std::vector<std::unique_ptr<Chunk>>&& chunks);
  EntityUUID SetEntity(std::unique_ptr<Entity> entity);
  void SetEntityChunkLoader(EntityUUID uuid);
  void SetBlock(const BlockID& block, BlockPos pos);
  void KillEntity(const EntityUUID& uuid);

  void ResetState();

 private:
  bool RequestLoad(ChunkPos pos);
  void loadSpawnChunks();
  void loadSummonEntitySurrounding(EntityUUID uuid);
  void loadSurroundedMovedEntityChunk();

  GameContext& context_;
  std::unique_ptr<WorldParameters> settings_;
  World* world_ = nullptr;
  FastHashSet<ChunkPos> generating_chunk_;
  bool is_spawn_chunks_loaded_ = false;

  FastHashSet<EntityUUID>
      entity_chunk_loaders_;  // List of entities that force loads chunks

  // Track last known chunk for each entity to detect chunk changes
  FastHashMap<EntityUUID, ChunkPos> entity_last_chunk_;

  std::vector<EntityUUID> chunk_loader_queue_;

  // States
  std::vector<ChunkPos> chunk_request_;
  FastHashSet<ChunkPos> created_chunk_;
  std::vector<ChunkPos> created_chunk_arr_;
  FastHashMap<ChunkPos, std::vector<BlockPos>> changed_block_;
};
