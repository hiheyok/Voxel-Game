#include <glm/vec3.hpp>

#include "Level/Chunk/Chunk.h"
#include "Level/World/WorldUpdater.h"
#include "Level/World/World.h"
#include "Level/World/WorldParameters.h"
#include "Level/Light/LightStorage.h"
#include "Level/Entity/Entity.h"
#include "Level/Container/EntityContainer.h"

using namespace glm;

WorldUpdater::WorldUpdater(World* w, WorldParameters p) : 
    settings_{ std::make_unique<WorldParameters>( p ) }, 
    world_{ w } {}

void WorldUpdater::loadSummonEntitySurrounding(EntityUUID uuid) {
    Entity* e = world_->GetEntity(uuid);

    if (e == nullptr) {
        g_logger.LogError("WorldLoader::loadSummonEntitySurrounding", std::string("Entity with UUID " + std::to_string(uuid) + " not found"));
        return;
    }
    vec3 pos = e->properties_.position_ / 16.f;
    // vec3 velocity = e->Properties.Velocity;

    ivec3 initial_pos{ (int)pos.x, (int)pos.y, (int)pos.z };

    // TODO: Use light engine FIFO 
    std::deque<ivec3> fifo;

    fifo.push_back(initial_pos);
    while (!fifo.empty()) {

        ivec3 current_chunk_pos = fifo.front();
        fifo.pop_front();

        //Gets relative position from the entity
        ivec3 offset = initial_pos - current_chunk_pos;

        //Checks if it is in range
        if ((abs(offset.x) > settings_->horizontal_ticking_distance_) || (abs(offset.z) > settings_->horizontal_ticking_distance_))
            continue;
        if (abs(offset.y) > settings_->vertical_ticking_distance_)
            continue;

        bool isSuccess = RequestLoad(ChunkPos{ current_chunk_pos.x, current_chunk_pos.y, current_chunk_pos.z });
        if (!isSuccess) continue;

        for (int side = 0; side < 6; side++) {

            ivec3 neighbor_chunk_pos = current_chunk_pos;

            neighbor_chunk_pos[side >> 1] += (side & 0b1) * 2 - 1;

            fifo.push_back(neighbor_chunk_pos);
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
        std::lock_guard<std::mutex> other_lock{ other_lock_ };
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
    std::vector<ivec3> center_position_list;
    std::vector<vec3> center_velocity_list;

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

        center_position_list.emplace_back(x, y, z);
        center_velocity_list.emplace_back(entity->properties_.velocity_);
    }

    if (center_position_list.empty()) return;

    ivec3 axisTickingDistance{ settings_->horizontal_ticking_distance_, settings_->vertical_ticking_distance_, settings_->horizontal_ticking_distance_ };

    int chunk_padding = 4;

    for (size_t i = 0; i < center_position_list.size(); i++) {
        ivec3 pos = center_position_list[i];
        vec3 vel = center_velocity_list[i];

        for (int j = 0; j < 3; j++) {
            int side = 0;

            if (vel[j] > 0) {
                side = 1;
            }

            if (vel[j] < 0) {
                side = -1;
            }

            if (side == 0) continue;

            int uDistance = axisTickingDistance[(j + 1) % 3] + chunk_padding;
            int vDistance = axisTickingDistance[(j + 2) % 3] + chunk_padding;
            int sideDistanceOffset = axisTickingDistance[j] + 1;

            for (int u = -uDistance; u <= uDistance; u++) {
                for (int v = -vDistance; v <= uDistance; v++) {
                    ivec3 test_position = pos;

                    test_position[(j + 1) % 3] += u;
                    test_position[(j + 2) % 3] += v;
                    test_position[j] += sideDistanceOffset * side;

                    //Test if it exist of generating
                    bool isSuccess = RequestLoad(ChunkPos{ test_position.x, test_position.y, test_position.z });
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
                if (!RequestLoad(ChunkPos{ x, y, z })) {
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
    return world_->GetEntityContainer()->GetSpawnedEntities();
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
    ChunkPos chunk_pos = pos;

    std::unique_lock<std::mutex> lock{ updated_chunk_lock_ };
    if (!updated_chunk_.contains(chunk_pos)) {
        updated_chunk_.insert(chunk_pos);
        updated_chunk_arr_.emplace_back(chunk_pos);
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
    std::vector<ChunkPos> updated_pos;
    updated_pos.reserve(lights.size());

    // TODO: add some optimizations to only update if it will affect the neighbors
    for (auto& light : lights) {
        Chunk* chunk = world_->GetChunk(light->position_);
        
        // See if it will actually make some changes
        if (*light == *chunk->lighting_) continue;

        // Update neighbor side too
        for (const auto& side : Directions<ChunkPos>()) {
            if (side.GetAxis() == Directions<ChunkPos>::kYAxis) {
                continue;
            }
            ChunkContainer* neighbor = chunk->GetNeighbor(side);
            if (neighbor == nullptr) continue;

            // Check if it should update the neighbor too
            bool update_neighbor = false;

            int ortho_pos = (kChunkDim - 1) * (~side.GetDirection() & 0b1);
            int relative_pos = (kChunkDim - 1) * (side.GetDirection() & 0b1);

            // Verify that it actually changed the side close to the neighboring chunk
            for (int i = 0; i < kChunkDim && !update_neighbor; ++i) {
                for (int y = 0; y < kChunkDim && !update_neighbor; ++y) {
                    if (side.GetAxis() == Directions<ChunkPos>::kXAxis) {
                        int curr_light = chunk->lighting_->GetLighting(BlockPos{ortho_pos, y, i});
                        if (curr_light != light->GetLighting(BlockPos{ortho_pos, y, i})) {
                            update_neighbor = true;
                        }
                    } else {
                        int curr_light = chunk->lighting_->GetLighting(BlockPos{i, y, ortho_pos});
                        if (curr_light != light->GetLighting(BlockPos{i, y, ortho_pos})) {
                            update_neighbor = true;
                        }
                    }
                }
            }

            // TODO: Then verify that it will produce changes in the neighbor side
            for (int i = 0; i < kChunkDim && !update_neighbor; ++i) {
                for (int y = 0; y < kChunkDim && !update_neighbor; ++y) {
                    if (side.GetAxis() == Directions<ChunkPos>::kXAxis) {
                        int currLight = chunk->lighting_->GetLighting(BlockPos{ortho_pos, y, i});
                        int neighborLight = neighbor->lighting_->GetLighting(BlockPos{relative_pos, y, i});
                        if (currLight != neighborLight - 1) {
                            update_neighbor = true;
                        }
                    }
                    else {
                        int currLight = chunk->lighting_->GetLighting(BlockPos{i, y, ortho_pos});
                        int neighborLight = neighbor->lighting_->GetLighting(BlockPos{i, y, relative_pos});
                        if (currLight != neighborLight - 1) {
                            update_neighbor = true;
                        }
                    }
                }
            }

            if (update_neighbor) {
                chunk->GetNeighbor(side)->SetLightDirty();
            }
        }
        updated_pos.push_back(light->position_);
        chunk->lighting_->ReplaceData(light->GetData());
    }
    
    std::unique_lock<std::mutex> lock{ updated_light_lock_ };
    for (const auto& pos : updated_pos) {
        if (!updated_light_.contains(pos)) {
            updated_light_.insert(pos);
            updated_light_arr_.emplace_back(pos);

            // Checks the side too 
        }
    }
}

void WorldUpdater::SetChunk(std::vector<std::unique_ptr<Chunk>> chunks) {
    std::vector<ChunkPos> updated_pos;
    updated_pos.reserve(chunks.size());

    for (auto& chunk : chunks) {
        updated_pos.push_back(chunk->position_);
        world_->SetChunk(std::move(chunk));
    }

    std::unique_lock<std::mutex> lock{ updated_chunk_lock_ };
    for (const auto& pos : updated_pos) {
        if (!updated_chunk_.contains(pos)) {
            updated_chunk_.insert(pos);
            updated_chunk_arr_.emplace_back(pos);

            for (const auto& offset : Directions<ChunkPos>()) {
                ChunkPos neighbor_pos = pos + offset;

                if (world_->CheckChunk(neighbor_pos) && !updated_chunk_.contains(neighbor_pos)) {
                    updated_chunk_arr_.push_back(neighbor_pos);
                }
            }
        }
    }
}

EntityUUID WorldUpdater::SetEntity(std::unique_ptr<Entity> entity) {
    bool is_chunk_loader = entity->properties_.is_chunk_loader_;
    EntityUUID uuid = world_->SetEntity(std::move(entity));
    if (is_chunk_loader) {
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