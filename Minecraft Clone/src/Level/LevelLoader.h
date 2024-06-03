#pragma once
#include "Level.h"
#include "../World/TerrainGeneration/Generator.h"

//Checks if the chunk is stored on disk, if not, asks the world generator to generate the chunks
class LevelLoader { //need to add save to disk later
private:
	ChunkGeneration worldGenerator; //Change this 
public:
	void Start(int threadCount) {
		worldGenerator.Start(threadCount);
	}

	void Stop() {
		worldGenerator.Stop();
	}

	void getRequestedChunks(std::vector<ChunkID> RequestedChunks) { //Add option for requested chunks dimension type and generator type later
		//Later this will check on the disk for chunks

		for (auto& id : RequestedChunks) {
			worldGenerator.Generate(id);
		}
	}

	std::vector<Chunk*> getGeneratedChunk() {
		return worldGenerator.GetOutput();
	}

};