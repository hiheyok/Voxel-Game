#include "Level/World/World.h"

#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Container/EntityContainer.h"
#include "Level/ECS/ServerECSManager.h"
#include "Level/Entity/Entity.h"
#include "Utils/LogUtils.h"

World::World(GameContext& context) : WorldInterface{context, true, true} {
  // Create server-side ECS manager with full capabilities
  ecs_manager_ = std::make_unique<ServerECSManager>(context, *this);
}

World::~World() = default;


void World::SetBlock(BlockID block, BlockPos pos) {
  if (!chunks_->SetBlock(block, pos)) {
    LOG_WARN("Tried to place block outside of the world");
  }
}

void World::SetChunk(std::unique_ptr<Chunk> chunk) {
  chunks_->InsertChunk(std::move(chunk));
}

EntityUUID World::SetEntity(std::unique_ptr<Entity> entity) {
  return entities_->AddEntity(std::move(entity));
}

void World::RemoveEntity(const EntityUUID& uuid) {
  entities_->RemoveEntity(uuid);
}
