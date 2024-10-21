#pragma once

#include "../Chunk/Chunk.h"
#include "../Chunk/ChunkColumn.h"
#include "../../Utils/Containers/skaHashmap.h"

typedef unsigned long long int RegionID;

struct Region { //32x32x32 Chunk Region
	ChunkColumn* region[32 * 32]{ nullptr };

	void AddChunk(Chunk* chunk, uint16_t x, uint16_t y, uint16_t z);
	void AddChunkGlobalPos(Chunk* chunk, int32_t x, int32_t y, int32_t z);

	void EraseChunk(uint16_t x, uint16_t y, uint16_t z);
	void EraseChunkGlobalPos(int32_t x, int32_t y, int32_t z);

	bool CheckChunk(uint16_t x, uint16_t y, uint16_t z);
	bool CheckChunkGlobalPos(int32_t x, int32_t y, int32_t z);

	Chunk* GetChunk(uint16_t x, uint16_t y, uint16_t z);
	Chunk* GetChunkGlobalPos(int32_t x, int32_t y, int32_t z);

	ChunkColumn* GetChunkColumn(uint16_t x, uint16_t z);
	ChunkColumn* GetChunkColumnGlobalPos(int32_t x, int32_t z);

	bool CheckChunkColumn(uint16_t x, uint16_t z);
	bool CheckChunkColumnGlobalPos(int32_t x, int32_t z);

	uint64_t AccessCount = 0;
};

class ChunkMap {
public:

	ChunkColumn* GetColumn(int x, int y, int z);

	bool CheckChunk(int x, int y, int z);

	BlockID GetBlockGlobal(int x, int y, int z);

	bool SetBlockGlobal(BlockID block, int x, int y, int z);

	void InsertChunk(Chunk* chunk);

	bool EraseChunk(int x, int y, int z);

	Chunk* GetChunk(int x, int y, int z);
private:
	//Input position is the chunk position
	Region* GetRegion(int x, int y, int z);

	ska::flat_hash_map<RegionID, Region*> LiveRegion;
};