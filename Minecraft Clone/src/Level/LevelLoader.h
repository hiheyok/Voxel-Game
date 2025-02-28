#pragma once
#include "TerrainGeneration/Generator.h"
#include "Lighting/LightEngine.h"

//Checks if the chunk is stored on disk, if not, asks the world generator to generate the chunks
class LevelLoader { //need to add save to disk later
private:
	size_t count = 0;
	ChunkGeneration worldGenerator; //Change this 
	LightingEngine lightEngine;
public:
	void Start(int worldGenThreadCount, int lightEngineThreadCount, WorldAccess* world, long long worldGenSeedIn);

	void Stop();

	int getChunkCount();

	void sendRequestedChunks(std::vector<ChunkPos> RequestedChunks, WorldGeneratorID worldGenTypeIn);

	std::vector<Chunk*> getGeneratedChunk();

	void sendRequestedLightUpdates(std::vector<ChunkColumnPos> RequestedLight);

	std::vector<ChunkLightingContainer*> getLightingInfomation();
};