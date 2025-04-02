#include "Level/LevelLoader.h"
#include "Level/TerrainGeneration/Generator.h"
#include "Level/Lighting/LightEngine.h"
#include "Level/TerrainGeneration/Biome/Biomes.h"
#include "Level/TerrainGeneration/Biome/BiomeProvider.h"

LevelLoader::LevelLoader() : 
    world_generator_{ std::make_unique<ChunkGeneration>() },
    light_engine_{ std::make_unique<LightingEngine>() } {

}
LevelLoader::~LevelLoader() = default;

void LevelLoader::Start(int worldGenThreadCount, int light_engine_thread_count_, WorldAccess* world, long long worldGenSeedIn) {
    Biomes::RegisterBiome();
    BiomeProvider::init(worldGenSeedIn, new ChunkGeneratorSettings);
    world_generator_->Start(worldGenThreadCount, worldGenSeedIn);
    light_engine_->Start(light_engine_thread_count_, world);
}

void LevelLoader::Stop() {
    world_generator_->Stop();
}

size_t LevelLoader::GetChunkCount() {
    return count;
}

void LevelLoader::SendRequestedChunks(std::vector<ChunkPos> requestedChunks, WorldGeneratorID worldGenTypeIn) { //Add option for requested chunks dimension type and generator type later
    //Later this will check on the disk for chunks
    world_generator_->Generate(std::move(requestedChunks), worldGenTypeIn);
}

std::vector<std::unique_ptr<Chunk>> LevelLoader::GetGeneratedChunk() {
    std::vector<std::unique_ptr<Chunk>> out = world_generator_->GetOutput();
    count += out.size();
    return out;
}

void LevelLoader::SendRequestedLightUpdates(std::vector<ChunkColumnPos> requestedLight) {
    //Later this will check on the disk for baked light info
    light_engine_->Generate(std::move(requestedLight));
}

std::vector<std::unique_ptr<LightStorage>> LevelLoader::GetLightingInfomation() {
    return light_engine_->GetOutput();
}

size_t LevelLoader::GetLightEngineQueueSize() {
    return light_engine_->GetQueueSize();
}