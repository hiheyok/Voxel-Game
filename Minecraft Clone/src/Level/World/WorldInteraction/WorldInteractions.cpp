#include <exception>

#include "WorldInteractions.h"
#include "WorldLoader.h"
#include "../Collusion/WorldCollusion.h"
#include "../WorldParameters.h"
#include "../../Chunk/Chunk.h"
#include "../../Chunk/Lighting/ChunkLighting.h"
#include "../../DataContainer/EntityContainer.h"
#include "../../Entity/Entity.h"

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

void WorldInteractions::summonEntity(Entity& entity) {
    if (world == nullptr) throw std::exception("Cannot summon entity. World is null");
    world->entities_->AddEntities(entity);

    if (entity.properties_.is_chunk_loader_)
        worldLoader_->AddEntityChunkLoader(entity.properties_.entity_uuid_);
}

std::vector<ChunkPos> WorldInteractions::GetUpdatedChunkPos() {
    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
    std::vector<ChunkPos> chunkIDs = {};
    chunkIDs.insert(chunkIDs.end(), updated_chunk_.begin(), updated_chunk_.end());
    updated_chunk_.clear();

    return chunkIDs;
}

std::vector<ChunkPos> WorldInteractions::GetRequestedLightUpdates() {
    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
    std::vector<ChunkPos> pos = std::move(light_updates_);
    light_updates_.clear();
    for (const ChunkPos& p : pos) {
        requested_light_update_[p].second = SIZE_MAX;
    }
    return pos;
}

std::vector<EntityProperty> WorldInteractions::GetUpdatedEntities() {
    FastHashMap<EntityUUID, EntityProperty> m = world->entities_->ClientGetEntityUpdate();
    std::vector<EntityProperty> properties = {};

    for (const auto& e : m) {
        properties.push_back(e.second);
    }

    return properties;
}

std::vector<EntityUUID> WorldInteractions::GetRemovedEntities() {
    FastHashSet<EntityUUID> m = world->entities_->getRemovedEntities();
    std::vector<EntityUUID> ids = {};
    ids.insert(ids.end(), m.begin(), m.end());
    return ids;
}

void WorldInteractions::RequestLightUpdate(const ChunkPos& pos) {
    ChunkColumnPos columnPos = pos;
    columnPos.y /= 32;
    int y = pos.y & 31;

    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };

    if (requested_light_update_.count(pos)) {
        auto& info = requested_light_update_[pos];

        int currY = info.first;
        size_t index = info.second;

        if (currY < y) {
            if (index == SIZE_MAX) {
                info.second = light_updates_.size();
                light_updates_.push_back(pos);
            }
            else {
                info.first = y;
                light_updates_[index].y = y;
            }
        }
    }
    else {
        requested_light_update_.emplace(columnPos, std::pair<int, size_t>{y, light_updates_.size()});
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

void WorldInteractions::UpdateLighting(std::unique_ptr<ChunkLightingContainer> chunkLighting) {
    ChunkPos pos = chunkLighting->position_;

    world->GetColumn(chunkLighting->position_);
    worldLoader_->ReplaceLightInfomation(std::move(chunkLighting));

    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
    if (!updated_chunk_.count(pos)) {
        updated_chunk_.insert(pos);
    }
    pos.y /= 32;

    requested_light_update_.erase(pos);
}

void WorldInteractions::UpdateLighting(std::vector<std::unique_ptr<ChunkLightingContainer>> chunkLighting) {
    std::stack<ChunkPos> chunkToUpdate;

    for (auto& chunk : chunkLighting) {
        chunkToUpdate.push(chunk->position_);
        worldLoader_->ReplaceLightInfomation(std::move(chunk));
    }

    std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };

    while (!chunkToUpdate.empty()) {
        ChunkPos pos = chunkToUpdate.top();
        chunkToUpdate.pop();
        if (!updated_chunk_.count(pos)) {
            updated_chunk_.insert(pos);
        }
        pos.y /= 32;
        // TODO: Fix this with Chunk Column ID
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
        g_logger.LogError("WorldInteractions::SetBlock", e.what());
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

void WorldInteractions::AddEntity(Entity& entity) {
    world->entities_->AddEntities(entity);
}

void WorldInteractions::AddEntity(Entity* entity) {
    world->entities_->AddEntities(entity);
}