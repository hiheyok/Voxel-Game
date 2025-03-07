#include "LevelLoader.h"

void LevelLoader::Start(int worldGenThreadCount, int light_engine_thread_count_, WorldAccess* world, long long worldGenSeedIn) {
    Biomes::RegisterBiome();
    BiomeProvider::init(worldGenSeedIn, new ChunkGeneratorSettings);
    world_generator_.Start(worldGenThreadCount, worldGenSeedIn);
    light_engine_.Start(light_engine_thread_count_, world);
}

void LevelLoader::Stop() {
    world_generator_.Stop();
}

size_t LevelLoader::GetChunkCount() {
    return count;
}

void LevelLoader::SendRequestedChunks(std::vector<ChunkPos> requestedChunks, WorldGeneratorID worldGenTypeIn) { //Add option for requested chunks dimension type and generator type later
    //Later this will check on the disk for chunks
    world_generator_.Generate(std::move(requestedChunks), worldGenTypeIn);
}

std::vector<std::unique_ptr<Chunk>> LevelLoader::GetGeneratedChunk() {
    std::vector<std::unique_ptr<Chunk>> out = world_generator_.GetOutput();
    count += out.size();
    return out;
}

void LevelLoader::SendRequestedLightUpdates(std::vector<ChunkColumnPos> requestedLight) {
    //Later this will check on the disk for baked light info
    light_engine_.Generate(std::move(requestedLight));
}

std::vector<std::unique_ptr<ChunkLightingContainer>> LevelLoader::GetLightingInfomation() {
    return light_engine_.GetOutput();
}

size_t LevelLoader::GetLightEngineQueueSize() {
    return light_engine_.GetQueueSize();
}