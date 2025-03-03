#include "LevelLoader.h"

using namespace std;

void LevelLoader::Start(int worldGenThreadCount, int light_engine_thread_count_, WorldAccess* world, long long worldGenSeedIn) {
	Biomes::RegisterBiome();
	BiomeProvider::init(worldGenSeedIn, new ChunkGeneratorSettings);
	world_generator_.Start(worldGenThreadCount, worldGenSeedIn);
	light_engine_.Start(light_engine_thread_count_, world);
}

void LevelLoader::Stop() {
	world_generator_.Stop();
}

size_t LevelLoader::getChunkCount() {
	return count;
}

void LevelLoader::SendRequestedChunks(vector<ChunkPos> requestedChunks, WorldGeneratorID worldGenTypeIn) { //Add option for requested chunks dimension type and generator type later
	//Later this will check on the disk for chunks
	world_generator_.Generate(std::move(requestedChunks), worldGenTypeIn);
}

vector<Chunk*> LevelLoader::GetGeneratedChunk() {
	vector<Chunk*> out = world_generator_.GetOutput();
	count += out.size();
	return out;
}

void LevelLoader::SendRequestedLightUpdates(vector<ChunkColumnPos> requestedLight) {
	//Later this will check on the disk for baked light info
	light_engine_.Generate(std::move(requestedLight));
}

vector<std::shared_ptr<ChunkLightingContainer>> LevelLoader::GetLightingInfomation() {
	return light_engine_.GetOutput();
}