#include <glm/vec3.hpp>

#include "Level/Chunk/Chunk.h"
#include "Level/World/WorldInteraction/WorldLoader.h"
#include "Level/World/World.h"
#include "Level/World/WorldParameters.h"
#include "Level/Chunk/Lighting/LightStorage.h"
#include "Level/Entity/Entity.h"
#include "Level/Container/EntityContainer.h"

using namespace glm;

WorldLoader::WorldLoader(World* w, WorldParameters p) : settings_{ std::make_unique<WorldParameters>( p ) }, world{ w } {}

void WorldLoader::loadSummonEntitySurrounding(EntityUUID uuid) {
    Entity* e = world->entities_->GetEntity(uuid);

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

bool WorldLoader::RequestLoad(const ChunkPos& pos) {
    ChunkPos p = pos;
    if (world->CheckChunk(p))
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
void WorldLoader::loadSurroundedMovedEntityChunk() {
    std::vector<ivec3> centerPositionList = {};
    std::vector<vec3> centerVelocityList = {};

    //Get entity chunk position

    for (const auto& e : entity_chunk_loaders_) {
        Entity* entity = world->entities_->GetEntity(e);

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

void WorldLoader::loadSpawnChunks() {
    if (world == nullptr)
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

WorldAccess* WorldLoader::GetWorld() const {
    return static_cast<WorldAccess*>(world);
}

//TODO: Fix me
void WorldLoader::ReplaceLightInfomation(std::unique_ptr<LightStorage> lighting) {
    const ChunkPos& pos = lighting->position_;
    Chunk* chunk = world->GetChunk(pos);
    chunk->lighting_->ReplaceData(lighting->GetData());
}

std::vector<ChunkPos> WorldLoader::GetRequestedChunks() {
    std::lock_guard<std::mutex> lock{ lock_ };

    std::vector<ChunkPos> tmp = std::move(chunk_request_);
    chunk_request_.clear();

    return tmp;

}

void WorldLoader::AddEntityChunkLoader(EntityUUID uuid) {
    entity_chunk_loaders_.insert(uuid);
    loadSummonEntitySurrounding(uuid);
}

void WorldLoader::DeleteEntityChunkLoader(EntityUUID uuid) {
    if (!entity_chunk_loaders_.count(uuid))
        g_logger.LogError("WorldLoader::DeleteEntityChunkLoader", std::string("Could not find entity with UUID " + std::to_string(uuid)));

    entity_chunk_loaders_.erase(uuid);
}

bool WorldLoader::CheckEntityExistChunkLoader(EntityUUID uuid) const {
    return entity_chunk_loaders_.count(uuid);
}

void WorldLoader::Load() {
    if (!is_spawn_chunks_loaded_) loadSpawnChunks();

    loadSurroundedMovedEntityChunk();
}

void WorldLoader::AddChunk(std::unique_ptr<Chunk> chunk) {
    const ChunkPos& pos = chunk->position_;

    {
        std::lock_guard<std::mutex> otherLock{ other_lock_ };
        if (generating_chunk_.count(pos)) generating_chunk_.erase(pos);
    }

    world->SetChunk(std::move(chunk));
}