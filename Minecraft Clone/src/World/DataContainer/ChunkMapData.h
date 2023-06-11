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

		for (int axis = 0; axis < 3; axis++) {
			for (int face = 0; face < 2; face++) {
				int p[3]{ x ,y ,z }; // p[3] is just the position of the neighboring chunk

				p[axis] += (-2 * face) + 1; 

				if (CheckChunk(p[0], p[1], p[2])) {
					GetChunk(x, y, z).SetNeighbor((ChunkContainer*)&GetChunk(p[0], p[1], p[2]), axis * 2 + face);
					GetChunk(p[0], p[1], p[2]).SetNeighbor((ChunkContainer*)&GetChunk(x, y, z), axis * 2 - face + 1);
				}
			}
		}
	}

	Chunk& GetChunk(int x, int y, int z) {
		return Data[getChunkID(x, y, z)];
	}

	Chunk& GetChunk(ChunkID ID) {
		return Data[ID];
	}

	BlockID GetBlockGlobal(int x, int y, int z) {

		//c[3] is the position of the chunk
		//l[3] is the local position of a block inside a chunk

		int c[3]{ (int)floor((float)x / 16.f) ,(int)floor((float)y / 16.f) ,(int)floor((float)z / 16.f) };

		int l[3]{ x - c[0] * 16 ,y - c[1] * 16,z - c[2] * 16 };

		if (CheckChunk(c[0], c[1], c[2])) {
			BlockID block = GetChunk(c[0], c[1], c[2]).GetBlock(l[0], l[1], l[2]);

			return block;
		}

		return NULL_BLOCK;
	}

	bool ChangeBlockGlobal(BlockID block, int x, int y, int z) {
		int ChunkPos[3]{ (int)floor((float)x / 16.f) ,(int)floor((float)y / 16.f) ,(int)floor((float)z / 16.f) };

		int LocalBlockPos[3]{ x - ChunkPos[0] * 16 ,y - ChunkPos[1] * 16,z - ChunkPos[2] * 16 };

		if (CheckChunk(ChunkPos[0], ChunkPos[1], ChunkPos[2])) {
			Data[getChunkID(ChunkPos[0], ChunkPos[1], ChunkPos[2])].Blocks.ChangeBlock(block, LocalBlockPos[0], LocalBlockPos[1], LocalBlockPos[2]);
			Data[getChunkID(ChunkPos[0], ChunkPos[1], ChunkPos[2])].isEmpty = false;
			return true;
		}

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

	Chunk& operator[](ChunkID id) {
		return Data[id];
	}

private:
	std::unordered_set<ChunkID> ChunksBeingUsed;
	std::unordered_map<ChunkID, Chunk> Data;
	std::unordered_set<ChunkID> ChunkIDContainer;
	std::thread MainWorldThread;
};