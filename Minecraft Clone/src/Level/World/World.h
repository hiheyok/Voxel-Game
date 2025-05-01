#pragma once
#include <memory>

#include "Level/World/CollusionDetector.h"
#include "Level/World/WorldInterface.h"
#include "Level/World/WorldParameters.h"

class ChunkMap;
class EntityContainer;

class World : public WorldInterface {
 public:
  World();
  ~World();

  void SetChunk(std::unique_ptr<Chunk> chunk);
  EntityUUID SetEntity(std::unique_ptr<Entity> entity);
  void SetBlock(BlockID block, BlockPos pos);
  void RemoveEntity(const EntityUUID& uuid);
};
