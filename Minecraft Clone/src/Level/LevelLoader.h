#pragma once
#include "TerrainGeneration/Generator.h"
#include "Lighting/LightEngine.h"

//Checks if the chunk is stored on disk, if not, asks the world generator to generate the chunks
class LevelLoader { //need to add save to disk later
private:
	size_t count = 0; // TODO: Rename
	ChunkGeneration world_generator_; //Change this 
	LightingEngine light_engine_;
public:
	void Start(int worldGenThreadCount, int light_engine_thread_count_, WorldAccess* world, long long worldGenSeedIn);

	void Stop();

	size_t getChunkCount(); // TODO: Rename

	void SendRequestedChunks(std::vector<ChunkPos> requestedChunks, WorldGeneratorID worldGenTypeIn);

	std::vector<Chunk*> GetGeneratedChunk();

	void SendRequestedLightUpdates(std::vector<ChunkColumnPos> requestedLight);

	std::vector<std::shared_ptr<ChunkLightingContainer>> GetLightingInfomation();
};