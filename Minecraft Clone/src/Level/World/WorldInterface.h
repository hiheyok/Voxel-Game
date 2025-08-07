#pragma once
#include <memory>
#include <vector>

#include "Core/Typenames.h"
#include "Level/World/WorldParameters.h"

class EntityContainer;
class ChunkMap;
class Chunk;
class GameContext;

struct Entity;

// Allows for read only
class WorldInterface {
 public:
  explicit WorldInterface(GameContext&);
  virtual ~WorldInterface();

  BlockID GetBlock(BlockPos pos) const;
  Entity* GetEntity(EntityUUID) const;
  Chunk* GetChunk(ChunkPos ID) const;
  bool CheckChunk(ChunkPos ID) const;
  std::vector<Chunk*> GetAllChunks() const;

  ChunkMap* GetChunkMap() const;
  EntityContainer* GetEntityContainer() const;

  GameContext& context_;
  WorldParameters parameters;

 protected:
  std::unique_ptr<ChunkMap> chunks_;
  std::unique_ptr<EntityContainer> entities_;
};
