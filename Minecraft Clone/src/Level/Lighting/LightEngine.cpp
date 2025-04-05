#include "Utils/Containers/FIFOQueue.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/Heightmap/Heightmap.h"
#include "Level/Chunk/Lighting/LightStorage.h"
#include "Level/Lighting/LightEngine.h"
#include "Level/World/WorldDataAccess.h"


static thread_local FixedFIFOQueue<uint16_t> FIFOQueues;

constexpr const int8_t DIRECTION_OFFSETS[6][3] = {
    {-1, 0, 0}, {1, 0, 0},  // X: Left, Right
    {0, -1, 0}, {0, 1, 0},  // Y: Down, Up
    {0, 0, -1}, {0, 0, 1}   // Z: Back, Front
};

// Pack light node data into uint16_t
inline uint16_t PackLightNode(uint8_t x, uint8_t y, uint8_t z, uint8_t light) {
    return x | (y << 4) | (z << 8) | (light << 12);
}

// Unpack light node data
inline void UnpackLightNode(uint16_t node, uint8_t& x, uint8_t& y, uint8_t& z, uint8_t& light) {
    x = node & 0xF;
    y = (node >> 4) & 0xF;
    z = (node >> 8) & 0xF;
    light = (node >> 12) & 0xF;
}

void LightEngine::IncreaseLightLevel(std::unique_ptr<LightStorage>& container, uint8_t lvl, int x, int y, int z) {
    uint8_t curr = container->GetLighting(x, y, z);
    if (curr < lvl)
        container->EditLight(x, y, z, lvl);
}

void LightEngine::WorkOnChunkSkylight(Chunk* chunk, std::unique_ptr<LightStorage>& light) {
    const Heightmap& heightmap = *chunk->heightmap_.get();
    FIFOQueues.resetData();

    for (int x = 0; x < kChunkDim; x++) {
        for (int z = 0; z < kChunkDim; z++) {
            int h = heightmap.Get(x, z); // it will try to find pivot points

            if (h == kChunkDim)
                continue;

            FIFOQueues.push(PackLightNode(x, kChunkDim - 1, z, 15));
        }
    }

    while (!FIFOQueues.IsEmpty()) {
        //Get node
        uint16_t node = FIFOQueues.get();
        uint8_t nodeX, nodeY, nodeZ, nodeLight;

        UnpackLightNode(node, nodeX, nodeY, nodeZ, nodeLight);

        if (heightmap.Get(nodeX, nodeZ) <= nodeY) {
            nodeLight = 15;
        }

        if (light->GetLighting(nodeX, nodeY, nodeZ) >= nodeLight) {
            continue;
        }
        //Set node light level
        IncreaseLightLevel(light, nodeLight, nodeX, nodeY, nodeZ);

        if (chunk->GetBlockUnsafe(nodeX, nodeY, nodeZ) != g_blocks.AIR) {
            continue;
        }

        if (nodeLight == 0) continue;

        //Spread
        for (int side = 0; side < 6; side++) {

            int direction = side >> 1;
            int face = side & 0b1; //1 = Front 0 = Back

            if ((direction == 1) && (face == 1))
                continue; //skip Up direction

            // Case to handle the down direction
            if (direction == 1 && face == 0) {
                if (nodeY != 0) {
                    FIFOQueues.push(PackLightNode(nodeX, nodeY - 1, nodeZ, nodeLight));
                }
                continue;
            }

            uint8_t nx = nodeX + DIRECTION_OFFSETS[side][0];
            uint8_t ny = nodeY + DIRECTION_OFFSETS[side][1];
            uint8_t nz = nodeZ + DIRECTION_OFFSETS[side][2];

            int8_t newLight = nodeLight - 1;

            //Check if it is in the chunk first
            if (((nx | ny | nz) >> 4))
                continue;

            //Check if the light level is more or less
            int currLvl = light->GetLighting(nx, ny, nz);

            if (currLvl + 2 > newLight)
                continue;

            FIFOQueues.push(PackLightNode(nx, ny, nz, newLight));

        }
    }
}

std::unique_ptr<LightStorage> LightEngine::Worker(const ChunkPos& pos) {
    if (!FIFOQueues.IsInitialized())
        FIFOQueues.setSize(LightEngine::DEFAULT_FIFO_QUEUE_SIZE);

    Chunk* chunk = world_->GetChunk(pos);

    Heightmap heightmap = *chunk->heightmap_;

    std::unique_ptr<LightStorage> lighting = std::make_unique<LightStorage>();
    lighting->ResetLightingCustom(4);
    lighting->position_.set(pos.x, pos.y, pos.z);

    WorkOnChunkSkylight(chunk, lighting);

    return lighting;
}

void LightEngine::Generate(std::vector<ChunkPos> tasks) {
    lighting_thread_pool_->SubmitTask(tasks);
}

std::vector<std::unique_ptr<LightStorage>> LightEngine::GetOutput() {
    std::vector<std::unique_ptr<LightStorage>> out = lighting_thread_pool_->GetOutput();
    return out;
}

void LightEngine::Stop() {
    lighting_thread_pool_->Stop();
}

void LightEngine::Start(int lightEngineThreadsCount, WorldAccess* w) {
    lighting_thread_pool_ = std::make_unique<ThreadPool<ChunkPos,
        std::unique_ptr<LightStorage>>>(lightEngineThreadsCount,
            "Light Engine",
            [this](const ChunkPos& input) -> std::unique_ptr<LightStorage> {
                return Worker(input);
            },
            100);

    world_ = w;
}

void LightEngine::QueueChunk(const ChunkPos& pos) {
    lighting_thread_pool_->SubmitTask(pos);
}

size_t LightEngine::GetQueueSize() {
    return lighting_thread_pool_->GetQueueSize();
}