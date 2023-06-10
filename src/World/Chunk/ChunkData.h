#pragma once
#include "../DataContainer/BlockData.h"

#define PX 0x00
#define PY 0x01
#define PZ 0x02
#define NX 0x03
#define NY 0x04
#define NZ 0x05

class ChunkContainer {
	
public:
	
	void SetNeighbor(ChunkContainer* Neighbor, unsigned int Side) {
		Neighbors[Side] = Neighbor;
	}

	ChunkContainer* GetNeighbor(unsigned int Side) {
		return Neighbors[Side];
	}
	
	int x = NULL, y = NULL, z = NULL;

	bool isEmpty = true;

	ChunkContainer* Neighbors[6]{nullptr};

	BlockContainer Blocks;
};