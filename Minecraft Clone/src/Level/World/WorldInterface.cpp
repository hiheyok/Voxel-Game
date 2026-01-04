#include "Level/World/WorldInterface.h"

#include <memory>
#include <vector>

#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Container/EntityContainer.h"
#include "Level/ECS/IECSManager.h"

WorldInterface::WorldInterface(GameContext& context, bool neighbor_update,
                               bool heightmap_update)
    : context_{context},
      chunks_{std::make_unique<ChunkMap>(context, true, true)},
      entities_{std::make_unique<EntityContainer>()} {
  // ECS manager is created by derived classes (World or ClientCache)
}

WorldInterface::~WorldInterface() = default;

BlockID WorldInterface::GetBlock(BlockPos pos) const {
  return chunks_->GetBlock(pos);
}

Chunk& WorldInterface::GetChunk(ChunkPos pos) const {
  return chunks_->GetChunk(pos);
}

std::vector<Chunk*> WorldInterface::GetAllChunks() const {
  return chunks_->GetAllChunks();
}

bool WorldInterface::CheckChunk(ChunkPos pos) const {
  return chunks_->CheckChunk(pos);
}

Entity* WorldInterface::GetEntity(EntityUUID uuid) const {
  return entities_->GetEntity(uuid);
}

ChunkMap* WorldInterface::GetChunkMap() const { return chunks_.get(); }

EntityContainer* WorldInterface::GetEntityContainer() const {
  return entities_.get();
}

IECSManager& WorldInterface::GetECSManager() { return *ecs_manager_; }
