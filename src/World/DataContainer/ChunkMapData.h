#pragma once

#include "../Chunk/Chunk.h"

#include "../../Utils/Math/ChunkID/ChunkID.h"
#include <unordered_map>

class ChunkMap {
public:
	void InsertChunk(Chunk chunk, int x, int y, int z) {
		
		ChunkID ID = getChunkID(x, y, z);
		
		Data[ID] = chunk;

		if (Check(x, y, z - 1)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z - 1), NZ);
			GetChunk(x, y, z - 1).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), PZ);
		}
		if (Check(x, y, z + 1)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z + 1), PZ);
			GetChunk(x, y, z + 1).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), NZ);
		}
		if (Check(x, y - 1, z)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y - 1, z), NY);
			GetChunk(x, y - 1, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), PY);
		}
		if (Check(x, y + 1, z)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y + 1, z), PY);
			GetChunk(x, y + 1, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), NY);
		}
		if (Check(x - 1, y, z)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x - 1, y, z), NX);
			GetChunk(x - 1, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), PX);
		}
		if (Check(x + 1, y, z)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x + 1, y, z), PX);
			GetChunk(x + 1, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), NX);
		}

	}

	Chunk& GetChunk(int x, int y, int z) {
		return Data[getChunkID(x, y, z)];
	}

	bool Check(int x, int y, int z) {
		return Data.count(getChunkID(x, y, z));
	}
private:
	std::unordered_map<ChunkID, Chunk> Data;
};