#pragma once
#include "../DataContainer/BlockData.h"
#include <glm/vec3.hpp>
#include "ChunkID.h"

#define PX 0x00
#define NX 0x01
#define PY 0x02
#define NY 0x03
#define PZ 0x04
#define NZ 0x05

typedef unsigned long long int ChunkID;

class ChunkContainer {
	
public:
	
	void SetNeighbor(ChunkContainer* Neighbor, unsigned int Side) {
		Neighbors[Side] = Neighbor;
	}

	void ClearNeighbors() {
		for (int i = 0; i < 6; i++) {
			Neighbors[i] = nullptr;
		}
	}

	BlockID GetBlock(int x, int y, int z);

	ChunkContainer* GetNeighbor(unsigned int Side) {
		return Neighbors[Side];
	}
	
	void SetPosition(int x, int y, int z) {
		Position.x = x;
		Position.y = y;
		Position.z = z;
		chunkID = getChunkID(x, y, z);
	}

	glm::ivec3 Position;

	ChunkID chunkID;

	bool isEmpty = true;

	ChunkContainer* Neighbors[6]{nullptr};

	BlockContainer Blocks;
};