#include <glm/vec3.hpp>

#include "Level/Chunk/Chunk.h"
#include "Level/World/WorldUpdater.h"
#include "Level/World/World.h"
#include "Level/World/WorldParameters.h"
#include "Level/Light/LightStorage.h"
#include "Level/Entity/Entity.h"
#include "Level/Container/EntityContainer.h"

using namespace glm;

WorldUpdater::WorldUpdater(World* w, WorldParameters p) : settings_{ std::make_unique<WorldParameters>( p ) }, world_{ w } {}

void WorldUpdater::loadSummonEntitySurrounding(EntityUUID uuid) {
    Entity* e = world_->GetEntity(uuid);

    if (e == nullptr)
        g_logger.LogError("WorldLoader::loadSummonEntitySurrounding", std::string("Entity with UUID " + std::to_string(uuid) + " not found"));
    vec3 pos = e->properties_.position_ / 16.f;
    // vec3 velocity = e->Properties.Velocity;

    ivec3 initalPos{ (int)pos.x, (int)pos.y, (int)pos.z };

    // TODO: Use light engine FIFO 
    std::deque<ivec3> FIFO;

    FIFO.push_back(initalPos);
    while (!FIFO.empty()) {

        ivec3 chunkPos = FIFO.front();
        FIFO.pop_front();

        //Gets relative position from the entity
        ivec3 offset = initalPos - chunkPos;

        //Checks if it is in range
        if ((abs(offset.x) > settings_->horizontal_ticking_distance_) || (abs(offset.z) > settings_->horizontal_ticking_distance_))
            continue;
        if (abs(offset.y) > settings_->vertical_ticking_distance_)
            continue;

        bool isSuccess = RequestLoad(ChunkPos{ chunkPos.x, chunkPos.y, chunkPos.z });
        if (!isSuccess) continue;

        for (int side = 0; side < 6; side++) {

            ivec3 newObj = chunkPos;

            newObj[side >> 1] += (side & 0b1) * 2 - 1;

            FIFO.push_back(newObj);
        }
    }

}

bool WorldUpdater::RequestLoad(const ChunkPos& pos) {
    ChunkPos p = pos;
    if (world_->CheckChunk(p))
        return false;

    if (tall_generation_)
        p.y /= 16;

    {
        std::lock_guard<std::mutex> otherLock{ other_lock_ };
        if (generating_chunk_.count(p))
            return false;

        //Request chunk
        generating_chunk_.insert(p);
    }

    

    std::lock_guard<std::mutex> lock{ lock_ };
    chunk_request_.push_back(p);
    return true;
}

//Only work on loading chunks for not. Unloading for later
void WorldUpdater::loadSurroundedMovedEntityChunk() {
    std::vector<ivec3> centerPositionList = {};
    std::vector<vec3> centerVelocityList = {};

    //Get entity chunk position

    for (const auto& e : entity_chunk_loaders_) {
        Entity* entity = world_->GetEntity(e);

        if (entity == nullptr)
            g_logger.LogError("WorldLoader::loadSurroundedMovedEntityChunk", std::string("Entity with UUID " + std::to_string(e) + " not found"));

        int x = static_cast<int>(entity->properties_.position_.x / 16.f);
        int y = static_cast<int>(entity->properties_.position_.y / 16.f);
        int z = static_cast<int>(entity->properties_.position_.z / 16.f);

        if ((entity->properties_.velocity_.x == 0.f) && (entity->properties_.velocity_.y == 0.f) && (entity->properties_.velocity_.z == 0.f)) {
            continue;
        }

        centerPositionList.emplace_back(x, y, z);
        centerVelocityList.emplace_back(entity->properties_.velocity_);
    }

    if (centerPositionList.empty()) return;

    ivec3 axisTickingDistance{ settings_->horizontal_ticking_distance_, settings_->vertical_ticking_distance_, settings_->horizontal_ticking_distance_ };

    int chunkPadding = 4;

    for (size_t i = 0; i < centerPositionList.size(); i++) {
        ivec3 pos = centerPositionList[i];
        vec3 vel = centerVelocityList[i];

        for (int j = 0; j < 3; j++) {
            int side = 0;

            if (vel[j] > 0) {
                side = 1;
            }

            if (vel[j] < 0) {
                side = -1;
            }

            if (side == 0) continue;

            int uDistance = axisTickingDistance[(j + 1) % 3] + chunkPadding;
            int vDistance = axisTickingDistance[(j + 2) % 3] + chunkPadding;
            int sideDistanceOffset = axisTickingDistance[j] + 1;

            for (int u = -uDistance; u <= uDistance; u++) {
                for (int v = -vDistance; v <= uDistance; v++) {
                    ivec3 testPosition = pos;

                    testPosition[(j + 1) % 3] += u;
                    testPosition[(j + 2) % 3] += v;
                    testPosition[j] += sideDistanceOffset * side;

                    //Test if it exist of generating
                    bool isSuccess = RequestLoad(ChunkPos{ testPosition.x, testPosition.y, testPosition.z });
                    if (!isSuccess) continue;
                }
            }




        }
    }
}

void WorldUpdater::loadSpawnChunks() {
    if (world_ == nullptr)
        g_logger.LogError("WorldLoader::loadSpawnChunks", "World is not initialized. Couldn't set spawn chunks");
    for (long long int x = -settings_->spawn_chunk_horizontal_radius_; x <= settings_->spawn_chunk_horizontal_radius_; x++) {
        for (long long int z = -settings_->spawn_chunk_horizontal_radius_; z <= settings_->spawn_chunk_horizontal_radius_; z++) {
            for (long long int y = -settings_->spawn_chunk_vertical_radius_; y <= settings_->spawn_chunk_vertical_radius_; y++) {
                bool isSuccess = RequestLoad(ChunkPos{ x, y, z });

                if (!isSuccess) {
                    continue;
                }

            }
        }
    }

    is_spawn_chunks_loaded_ = true;
}

void WorldUpdater::DeleteEntityChunkLoader(EntityUUID uuid) {
    if (!entity_chunk_loaders_.count(uuid))
        g_logger.LogError("WorldLoader::DeleteEntityChunkLoader", std::string("Could not find entity with UUID " + std::to_string(uuid)));

    entity_chunk_loaders_.erase(uuid);
}

bool WorldUpdater::CheckEntityExistChunkLoader(EntityUUID uuid) const {
    return entity_chunk_loaders_.count(uuid);
}

void WorldUpdater::Load() {
    if (!is_spawn_chunks_loaded_) loadSpawnChunks();

    loadSurroundedMovedEntityChunk();
}



// Getters

std::vector<ChunkPos> WorldUpdater::GetUpdatedChunkPos() {
    std::unique_lock<std::mutex> lock{ updated_chunk_lock_ };
    std::vector<ChunkPos> out = std::move(updated_chunk_arr_);
    updated_chunk_arr_.clear();
    updated_chunk_.clear();
    return out;
}

std::vector<ChunkPos> WorldUpdater::GetUpdatedLightPos() {
    std::unique_lock<std::mutex> lock{ updated_light_lock_ };
    std::vector<ChunkPos> out = std::move(updated_light_arr_);
    updated_light_arr_.clear();
    updated_light_.clear();
    return out;
}

std::vector<ChunkPos> WorldUpdater::GetRequestedLightUpdates() {
    std::vector<Chunk*> chunks = world_->GetAllChunks();
    std::vector<ChunkPos> pos;

    for (const auto& chunk : chunks) {
        if (chunk->CheckLightDirty()) {
            pos.push_back(chunk->position_);
        }
    }
    return pos;
}

std::vector<ChunkPos> WorldUpdater::GetRequestedChunks() {
    std::lock_guard<std::mutex> lock{ lock_ };
    std::vector<ChunkPos> tmp = std::move(chunk_request_);
    chunk_request_.clear();
    return tmp;
}

std::vector<EntityProperty> WorldUpdater::GetSpawnedEntities() {
    return world_->GetEntityContainer()->GetSpawnedEntities(); // TODO: Create some internal stuff to handle this in the chunk updater
}

std::vector<EntityProperty> WorldUpdater::GetUpdatedEntities() {
    return world_->GetEntityContainer()->GetUpdatedEntities();
}

std::vector<EntityUUID> WorldUpdater::GetRemovedEntities() {
    return world_->GetEntityContainer()->GetRemovedEntities();
}
// Setters 

void WorldUpdater::SetBlock(const BlockID& block, const BlockPos& pos) {
    world_->SetBlock(block, pos);

    ChunkPos chunkPos = pos / kChunkDim;

    std::unique_lock<std::mutex> lock{ updated_chunk_lock_ };
    if (!updated_chunk_.contains(chunkPos)) {
        updated_chunk_.insert(chunkPos);
        updated_chunk_arr_.emplace_back(chunkPos);
    }
}

void WorldUpdater::SetEntityChunkLoader(EntityUUID uuid) {
    entity_chunk_loaders_.insert(uuid);
    loadSummonEntitySurrounding(uuid);
}

void WorldUpdater::SetLight(std::unique_ptr<LightStorage> light) {
    SetLight({ std::move(light) });
}


void WorldUpdater::SetChunk(std::unique_ptr<Chunk> chunk) {
    SetChunk({std::move(chunk)});
}

void WorldUpdater::SetLight(std::vector<std::unique_ptr<LightStorage>> lights) {
    std::vector<ChunkPos> updatedPos;
    updatedPos.reserve(lights.size());

    for (auto& light : lights) {
        Chunk* chunk = world_->GetChunk(light->position_);
        updatedPos.push_back(light->position_);
        chunk->lighting_->ReplaceData(light->GetData());
    }
    
    std::unique_lock<std::mutex> lock{ updated_light_lock_ };
    for (const auto& pos : updatedPos) {
        if (!updated_light_.contains(pos)) {
            updated_light_.insert(pos);
            updated_light_arr_.emplace_back(pos);

            // Checks the side too 
        }
    }
}

void WorldUpdater::SetChunk(std::vector<std::unique_ptr<Chunk>> chunks) {
    std::vector<ChunkPos> updatedPos;
    updatedPos.reserve(chunks.size());

    for (auto& chunk : chunks) {
        updatedPos.push_back(chunk->position_);
        world_->SetChunk(std::move(chunk));
    }

    std::unique_lock<std::mutex> lock{ updated_chunk_lock_ };
    for (const auto& pos : updatedPos) {
        if (!updated_chunk_.contains(pos)) {
            updated_chunk_.insert(pos);
            updated_chunk_arr_.emplace_back(pos);

            for (int side = 0; side < 6; side++) {
                ChunkPos neighborPos = pos;
                neighborPos.incrementSide(side, 1);

                if (world_->CheckChunk(neighborPos) && !updated_chunk_.contains(neighborPos)) {
                    updated_chunk_arr_.push_back(neighborPos);
                }
            }
        }
    }
}

EntityUUID WorldUpdater::SetEntity(std::unique_ptr<Entity> entity) {
    bool isChunkLoader = entity->properties_.is_chunk_loader_;
    EntityUUID uuid = world_->SetEntity(std::move(entity));
    if (isChunkLoader) {
        SetEntityChunkLoader(uuid);
    }
    return uuid;
}

void WorldUpdater::KillEntity(const EntityUUID& uuid) {
    world_->RemoveEntity(uuid);
    if (CheckEntityExistChunkLoader(uuid)) {
        DeleteEntityChunkLoader(uuid);
    }
}