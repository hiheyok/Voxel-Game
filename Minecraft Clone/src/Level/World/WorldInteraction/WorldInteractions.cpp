#include <exception>

#include "Level/World/WorldInteraction/WorldInteractions.h"
#include "Level/World/WorldInteraction/WorldLoader.h"
#include "Level/World/Collusion/WorldCollusion.h"
#include "Level/World/World.h"
#include "Level/World/WorldParameters.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/Lighting/LightStorage.h"
#include "Level/DataContainer/EntityContainer.h"
#include "Level/Entity/Entity.h"

WorldInteractions::WorldInteractions() = default;

WorldInteractions::WorldInteractions(World* w, WorldParameters parameters) {
    init(w, parameters);
}

void WorldInteractions::UseTallGeneration() {
    worldLoader_->tall_generation_ = true;
}

void WorldInteractions::init(World* w, WorldParameters parameters) {
    world = w;
    settings_ = std::make_unique<WorldParameters>(parameters);
    collusions_ = std::make_unique<WorldCollusionDetector>();
    worldLoader_ = new WorldLoader(w, parameters);
    collusions_->Initialize(w->chunk_container_.get());
}

std::vector<ChunkPos> WorldInteractions::GetUpdatedChunkPos() {
    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
    std::vector<ChunkPos> chunkIDs = {};
    chunkIDs.insert(chunkIDs.end(), updated_chunk_.begin(), updated_chunk_.end());
    updated_chunk_.clear();

    return chunkIDs;
}

std::vector<ChunkPos> WorldInteractions::GetUpdatedLightPos() {
    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
    std::vector<ChunkPos> out(updated_lighting_pos_.begin(), updated_lighting_pos_.end());
    updated_lighting_pos_.clear();
    return out;
}

std::vector<ChunkPos> WorldInteractions::GetRequestedLightUpdates() {
    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
    std::vector<ChunkPos> pos = std::move(light_updates_);
    light_updates_.clear();
    return pos;
}

std::vector<EntityProperty> WorldInteractions::GetSpawnedEntities() {
    return world->entities_->GetSpawnedEntities();
}

std::vector<EntityProperty> WorldInteractions::GetUpdatedEntities() {
    return world->entities_->GetUpdatedEntities();
}

std::vector<EntityUUID> WorldInteractions::GetRemovedEntities() {
    return world->entities_->GetRemovedEntities();
}

void WorldInteractions::RequestLightUpdate(const ChunkPos& pos) {
    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };

    if (!requested_light_update_.count(pos)) {
        requested_light_update_.insert(pos);
        light_updates_.push_back(pos);
    }
}

void WorldInteractions::KillEntity(EntityUUID id) {
    world->entities_->RemoveEntity(id);
    if (worldLoader_->CheckEntityExistChunkLoader(id)) {
        worldLoader_->DeleteEntityChunkLoader(id);
    }
}

void WorldInteractions::Update() {
    worldLoader_->Load();
}

void WorldInteractions::UpdateLighting(std::unique_ptr<LightStorage> chunkLighting) {
    ChunkPos pos = chunkLighting->position_;

    worldLoader_->ReplaceLightInfomation(std::move(chunkLighting));

    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
    if (!updated_lighting_pos_.count(pos)) {
        updated_lighting_pos_.insert(pos);
    }

    requested_light_update_.erase(pos);
}

void WorldInteractions::UpdateLighting(std::vector<std::unique_ptr<LightStorage>> chunkLighting) {
    std::stack<ChunkPos> chunkToUpdate;

    for (auto& chunk : chunkLighting) {
        chunkToUpdate.push(chunk->position_);
        worldLoader_->ReplaceLightInfomation(std::move(chunk));
    }

    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };

    while (!chunkToUpdate.empty()) {
        ChunkPos pos = chunkToUpdate.top();
        chunkToUpdate.pop();
        if (!updated_lighting_pos_.count(pos)) {
            updated_lighting_pos_.insert(pos);
        }

        requested_light_update_.erase(pos);

    }
}

void WorldInteractions::AddChunk(std::unique_ptr<Chunk> chunk) {
    std::stack<ChunkPos> chunkToUpdate;

    const ChunkPos& position = chunk->position_;

    world->SetChunk(std::move(chunk));
    RequestLightUpdate(position);
    chunkToUpdate.push(position);

    //Update neighbor chunks

    for (int side = 0; side < 6; side++) {
        ChunkPos neighborPos = position;
        neighborPos.incrementSide(side, 1);

        if (world->CheckChunk(neighborPos)) {
            chunkToUpdate.push(neighborPos);
        }
    }

    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };

    while (!chunkToUpdate.empty()) {
        ChunkPos pos = chunkToUpdate.top();
        chunkToUpdate.pop();
        if (!updated_chunk_.count(pos)) {
            updated_chunk_.insert(pos);
        }

    }
}

void WorldInteractions::AddChunks(std::vector<std::unique_ptr<Chunk>> chunks) {
    std::vector<ChunkPos> chunkToUpdate;
    FastHashSet<ChunkPos> duplicatedInputs;

    for (auto& chunk : chunks) {

        const ChunkPos& position = chunk->position_;

        chunkToUpdate.push_back(position);
        //Update neighbor chunks

        for (int side = 0; side < 6; side++) {
            ChunkPos neighborPos = position;
            neighborPos.incrementSide(side, 1);

            if (!duplicatedInputs.count(neighborPos) && world->CheckChunk(neighborPos)) {
                chunkToUpdate.push_back(neighborPos);
                duplicatedInputs.insert(neighborPos);
            }
        }
        RequestLightUpdate(position);
        world->SetChunk(std::move(chunk));
    }

    {
        std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
        for (const ChunkPos& pos : chunkToUpdate) {
            if (!updated_chunk_.count(pos)) {
                updated_chunk_.insert(pos);
            }

        }
    }
    chunkToUpdate.clear();
}

Chunk* WorldInteractions::GetChunk(const ChunkPos& pos) const {
    if (!world->CheckChunk(pos)) {
        return nullptr;
    }

    return world->GetChunk(pos);
}

void WorldInteractions::SetBlock(BlockID b, const BlockPos& bpos) {
    //UpdatedChunkLock.lock();
    try {
        int x = bpos.x;
        int y = bpos.y;
        int z = bpos.z;

        ChunkPos pos = ChunkPos{ x >> 4, y >> 4, z >> 4 };

        world->SetBlock(b, bpos);
        if (!updated_chunk_.count(pos)) {
            updated_chunk_.insert(pos);
        }

        int v[3]{ x % 16, y % 16, z % 16 };

        for (int axis_ = 0; axis_ < 3; axis_++) {
            ChunkPos p = pos;

            int direction = 0;

            if (v[axis_] == 15)
                direction = 1;
            if (v[axis_] == 0)
                direction = -1;

            if (direction == 0) continue;

            p[axis_] += direction;
            if (!updated_chunk_.count(p))
                updated_chunk_.insert(p);
        }


        RequestLightUpdate(pos);
    }
    catch (std::exception& e) {
        g_logger.LogWarn("WorldInteractions::SetBlock", e.what());
    }
}

BlockID WorldInteractions::GetBlock(const BlockPos& pos) {
    try {
        return world->GetBlock(pos);
    }
    catch (std::exception& e) {
        g_logger.LogError("WorldInteractions::GetBlock", e.what());
        return g_blocks.AIR;
    }
}

Entity* WorldInteractions::GetEntity(EntityUUID id) {
    return world->entities_->GetEntity(id);
}

EntityUUID WorldInteractions::AddEntity(std::unique_ptr<Entity> entity) {
    if (world == nullptr) throw std::exception("Cannot summon entity. World is null");

    bool isChunkLoader = entity->properties_.is_chunk_loader_;
    EntityUUID uuid = world->entities_->AddEntity(std::move(entity));

    if (isChunkLoader)
        worldLoader_->AddEntityChunkLoader(uuid);

    return uuid;
}