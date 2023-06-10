#pragma once

#include "../Chunk/Chunk.h"

#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#include <unordered_map>
#include <unordered_set>

class ChunkMap {
public:
	void InsertChunk(Chunk chunk, int x, int y, int z) {
		
		ChunkID ID = getChunkID(x, y, z);
		
		Data[ID] = chunk;
		ChunkIDContainer.insert(ID);

		if (CheckChunk(x, y, z - 1)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z - 1), NZ);
			GetChunk(x, y, z - 1).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), PZ);
		}
		if (CheckChunk(x, y, z + 1)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z + 1), PZ);
			GetChunk(x, y, z + 1).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), NZ);
		}
		if (CheckChunk(x, y - 1, z)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y - 1, z), NY);
			GetChunk(x, y - 1, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), PY);
		}
		if (CheckChunk(x, y + 1, z)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y + 1, z), PY);
			GetChunk(x, y + 1, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), NY);
		}
		if (CheckChunk(x - 1, y, z)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x - 1, y, z), NX);
			GetChunk(x - 1, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), PX);
		}
		if (CheckChunk(x + 1, y, z)) {
			GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x + 1, y, z), PX);
			GetChunk(x + 1, y, z).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), NX);
		}

	}

	Chunk& GetChunk(int x, int y, int z) {
		return Data[getChunkID(x, y, z)];
	}

	Chunk& GetChunk(ChunkID ID) {
		return Data[ID];
	}

	BlockID GetBlockGlobal(int x, int y, int z) {
		int ChunkX = (int)floor((float)x / 16.f);
		int ChunkY = (int)floor((float)y / 16.f);
		int ChunkZ = (int)floor((float)z / 16.f);

		UsingChunk(getChunkID(ChunkX, ChunkY, ChunkZ));

		int LocalX = x - ChunkX * 16;
		int LocalY = y - ChunkY * 16;
		int LocalZ = z - ChunkZ * 16;

		if (CheckChunk(ChunkX, ChunkY, ChunkZ)) {
			BlockID block = GetChunk(ChunkX, ChunkY, ChunkZ).GetBlock(LocalX, LocalY, LocalZ);

			FinishedChunk(getChunkID(ChunkX, ChunkY, ChunkZ));

			return block;
		}

		return NULL_BLOCK;
	}

	BlockID GetBlockGlobal(glm::ivec3 pos) {
		int ChunkX = (int)floor((float)pos.x / 16.f);
		int ChunkY = (int)floor((float)pos.y / 16.f);
		int ChunkZ = (int)floor((float)pos.z / 16.f);

		UsingChunk(getChunkID(ChunkX, ChunkY, ChunkZ));

		int LocalX = pos.x - ChunkX * 16;
		int LocalY = pos.y - ChunkY * 16;
		int LocalZ = pos.z - ChunkZ * 16;

		if (CheckChunk(ChunkX, ChunkY, ChunkZ)) {
			return GetChunk(ChunkX, ChunkY, ChunkZ).GetBlock(LocalX, LocalY, LocalZ);
		}
		return NULL_BLOCK;
	}

	bool ChangeBlockGlobal(BlockID block, int x, int y, int z) {
		int ChunkX = (int)floor((float)x / 16.f);
		int ChunkY = (int)floor((float)y / 16.f);
		int ChunkZ = (int)floor((float)z / 16.f);

		UsingChunk(getChunkID(ChunkX, ChunkY, ChunkZ));

		unsigned int LocalX = x - ChunkX * 16;
		unsigned int LocalY = y - ChunkY * 16;
		unsigned int LocalZ = z - ChunkZ * 16;

		if (CheckChunk(ChunkX, ChunkY, ChunkZ)) {
			Data[getChunkID(ChunkX, ChunkY, ChunkZ)].Blocks.ChangeBlock(block, LocalX, LocalY, LocalZ);
			Data[getChunkID(ChunkX, ChunkY, ChunkZ)].isEmpty = false;
			FinishedChunk(getChunkID(ChunkX, ChunkY, ChunkZ));
			return true;
		}

		FinishedChunk(getChunkID(ChunkX, ChunkY, ChunkZ));
		return false;

		
	}

	bool CheckChunk(int x, int y, int z) {
		return ChunkIDContainer.count(getChunkID(x, y, z));
	}

	void UsingChunk(ChunkID id) {
		ChunksBeingUsed.insert(id);
	}

	void FinishedChunk(ChunkID id) {
		ChunksBeingUsed.erase(id);
	}

	bool IsChunkInUse(ChunkID id) {
		ChunksBeingUsed.contains(id);
	}
private:
	std::unordered_set<ChunkID> ChunksBeingUsed;
	std::unordered_map<ChunkID, Chunk> Data;
	std::unordered_set<ChunkID> ChunkIDContainer;
	std::thread MainWorldThread;
};