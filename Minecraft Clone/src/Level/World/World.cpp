#include "Level/World/World.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Container/EntityContainer.h"
#include "Level/Entity/Entity.h"
#include "Level/Chunk/Chunk.h"

World::World() : WorldInterface{} {}

World::~World() = default;

void World::SetBlock(BlockID block, const BlockPos& pos) {
    if (!chunks_->SetBlock(block, pos)) {
        g_logger.LogError("World::SetBlock", "Tried to place block outside of the world");
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

