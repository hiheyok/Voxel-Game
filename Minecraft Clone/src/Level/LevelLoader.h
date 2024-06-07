#pragma once
#include "TerrainGeneration/Generator.h"
#include "Lighting/LightEngine.h"

//Checks if the chunk is stored on disk, if not, asks the world generator to generate the chunks
class LevelLoader { //need to add save to disk later
private:
	int count = 0;
	ChunkGeneration worldGenerator; //Change this 
	LightingEngine lightEngine;
public:
	void Start(int worldGenThreadCount, int lightEngineThreadCount, WorldAccess* world);

	void Stop();

	int getChunkCount();

	void sendRequestedChunks(std::vector<ChunkID> RequestedChunks);

	std::vector<Chunk*> getGeneratedChunk();

	void sendRequestedLightUpdates(std::vector<ChunkColumnID> RequestedLight);

	std::vector<ChunkLightingContainer*> getLightingInfomation();
};