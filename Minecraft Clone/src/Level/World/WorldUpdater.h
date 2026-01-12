#pragma once
#include <memory>
#include <mutex>
#include <tuple>
#include <vector>

#include "Core/Typenames.h"
#include "Level/World/ChunkLoader.h"

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
  WorldUpdater(GameContext&, World* w, WorldParameters p);

  void SetTallGeneration(bool val);
  bool IsTallGeneration() const;

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
  GameContext& context_;
  World* world_ = nullptr;
  
  std::unique_ptr<ChunkLoader> chunk_loader_;

  // States
  FastHashSet<ChunkPos> created_chunk_;
  std::vector<ChunkPos> created_chunk_arr_;
  FastHashMap<ChunkPos, std::vector<BlockPos>> changed_block_;
};
