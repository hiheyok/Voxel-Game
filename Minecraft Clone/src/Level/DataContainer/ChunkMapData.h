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

	bool CheckChunk(uint16_t x, uint16_t y, uint16_t z) const;
	bool CheckChunkGlobalPos(int32_t x, int32_t y, int32_t z) const;

	Chunk* GetChunk(uint16_t x, uint16_t y, uint16_t z) const;
	Chunk* GetChunkGlobalPos(int32_t x, int32_t y, int32_t z) const;

	ChunkColumn* GetChunkColumn(uint16_t x, uint16_t z) const;
	ChunkColumn* GetChunkColumnGlobalPos(int32_t x, int32_t z) const;

	bool CheckChunkColumn(uint16_t x, uint16_t z) const;
	bool CheckChunkColumnGlobalPos(int32_t x, int32_t z) const;

	uint64_t AccessCount = 0;
};

class ChunkMap {
public:

	ChunkColumn* GetColumn(const ChunkPos& pos) const;

	bool CheckChunk(const ChunkPos& pos) const;

	BlockID GetBlockGlobal(const BlockPos& pos);

	bool SetBlockGlobal(BlockID block, const BlockPos& pos);

	void InsertChunk(Chunk* chunk);

	bool EraseChunk(const ChunkPos& pos);

	Chunk* GetChunk(const ChunkPos& pos) const;
private:
	//Input position is the chunk position
	Region* GetRegion(const ChunkPos& pos) const;

	FastHashMap<RegionPos, Region*> LiveRegion;
};