#pragma once
#include "Level.h"
#include "../World/TerrainGeneration/Generator.h"

//Checks if the chunk is stored on disk, if not, asks the world generator to generate the chunks
class LevelLoader { //need to add save to disk later
private:
	int count = 0;
	ChunkGeneration worldGenerator; //Change this 
public:
	void Start(int threadCount) {
		worldGenerator.Start(threadCount);
	}

	void Stop() {
		worldGenerator.Stop();
	}

	int getChunkCount() {
		return count;
	}

	void getRequestedChunks(std::vector<ChunkID> RequestedChunks) { //Add option for requested chunks dimension type and generator type later
		//Later this will check on the disk for chunks

		worldGenerator.Generate(RequestedChunks);
	}

	std::vector<Chunk*> getGeneratedChunk() {
		std::vector<Chunk*> out = worldGenerator.GetOutput();
		count += out.size();
		return out;
	}

};