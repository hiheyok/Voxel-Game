#pragma once

#include "ChunkMapData.h"
#include "../Entity/Entity.h"

/*
Contains all of the world data and basic functions to manage it
	- Chunks
	- Entities
*/

class WorldData {
public:

	BlockID GetBlock(int x, int y, int z);
	bool SetBlock(BlockID block, int x, int y, int z);

	void SetChunk(Chunk chunk);
	bool CheckChunk(int x, int y, int z);
	bool EraseChunk(int x, int y, int z);

private:

	ChunkMap WorldChunkData;

};