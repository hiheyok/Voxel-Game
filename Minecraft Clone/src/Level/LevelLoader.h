#pragma once
#include "Level.h"
#include "TerrainGeneration/Generator.h"
#include "Lighting/LightEngine.h"

//Checks if the chunk is stored on disk, if not, asks the world generator to generate the chunks
class LevelLoader { //need to add save to disk later
private:
	int count = 0;
	ChunkGeneration worldGenerator; //Change this 
	LightingEngine lightEngine;
public:
	void Start(int threadCount, WorldAccess* world) {
		worldGenerator.Start(threadCount);
		lightEngine.Start(1, world);
	}

	void Stop() {
		worldGenerator.Stop();
	}

	int getChunkCount() {
		return count;
	}

	void sendRequestedChunks(std::vector<ChunkID> RequestedChunks) { //Add option for requested chunks dimension type and generator type later
		//Later this will check on the disk for chunks
		worldGenerator.Generate(RequestedChunks);
	}

	std::vector<Chunk*> getGeneratedChunk() {
		std::vector<Chunk*> out = worldGenerator.GetOutput();
		count += out.size();
		return out;
	}

	void sendRequestedLightUpdates(std::vector<ChunkColumnID> RequestedLight) {
		//Later this will check on the disk for baked light info
		lightEngine.Generate(RequestedLight);
	}

	std::vector<ChunkLightingContainer*> getLightingInfomation() {
		std::vector<ChunkLightingContainer*> out = lightEngine.GetOutput();
		return out;
	}

};