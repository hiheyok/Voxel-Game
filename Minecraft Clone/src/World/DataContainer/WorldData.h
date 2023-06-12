#pragma once

#include "ChunkMapData.h"
#include "../Entity/Entity.h"

#include <concurrent_unordered_set.h>

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
	bool CheckChunk(ChunkID id);

	Chunk& GetChunk(int x, int y, int z);
	Chunk& GetChunk(ChunkID id);

	Chunk GetCopyOfChunk(int x, int y, int z);

	bool EraseChunk(int x, int y, int z);

	void ChunkUpdate(int x, int y, int z);

	std::unordered_set<ChunkID> ChunksUpdated;

	std::mutex ChunkUpdateLock;

private:

	ChunkMap WorldChunkData;
};