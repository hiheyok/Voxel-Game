#pragma once

#include "../Chunk/Chunk.h"

#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>

#include <unordered_map>
#include <unordered_set>

#include <atomic>

typedef unsigned long long int RegionID;

struct Region { //32x32x32 Chunk Region
	Chunk* region[32 * 32 * 32]{ nullptr };

	void AddChunk(Chunk* chunk, uint16_t x, uint16_t y, uint16_t z);
	void AddChunkGlobalPos(Chunk* chunk, int32_t x, int32_t y, int32_t z);

	void EraseChunk(uint16_t x, uint16_t y, uint16_t z);
	void EraseChunkGlobalPos(int32_t x, int32_t y, int32_t z);

	bool CheckChunk(uint16_t x, uint16_t y, uint16_t z);
	bool CheckChunkGlobalPos(int32_t x, int32_t y, int32_t z);

	Chunk* GetChunk(uint16_t x, uint16_t y, uint16_t z);
	Chunk* GetChunkGlobalPos(int32_t x, int32_t y, int32_t z);
};

class ChunkMap {
public:

	bool CheckChunk(int x, int y, int z);
	bool CheckChunk(ChunkID id);

	BlockID GetBlockGlobal(int x, int y, int z);

	bool SetBlockGlobal(BlockID block, int x, int y, int z);

	

	void InsertChunk(Chunk* chunk);

	bool EraseChunk(int x, int y, int z);

	Chunk* GetChunk(int x, int y, int z);
private:
	//Input position is the chunk position
	Region* GetRegion(int x, int y, int z);

	std::unordered_map<RegionID, Region> LiveRegion;
};