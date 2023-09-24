#pragma once
#include "../DataContainer/BlockData.h"
#include "../TerrainGeneration/Structures/Structure.h"
#include <glm/vec3.hpp>
#include "ChunkID.h"
#include <atomic>

#ifndef CHUNKDATA_H
#define CHUNKDATA_H



#define PX 0x00
#define NX 0x01
#define PY 0x02
#define NY 0x03
#define PZ 0x04
#define NZ 0x05

typedef unsigned long long int ChunkID;


class ChunkContainer {
	
public:
	
	void SetNeighbor(ChunkContainer* Neighbor, unsigned int Side);

	void ClearNeighbors();

	BlockID GetBlock(int x, int y, int z);
	BlockID GetBlockUnsafe(int x, int y, int z);

	void SetBlock(BlockID block, int x, int y, int z);
	void SetBlockUnsafe(BlockID block, int x, int y, int z);

	void Use();

	void Unuse();

	inline ChunkContainer* GetNeighbor(unsigned int Side);
	
	void SetPosition(int x, int y, int z);

	glm::ivec3 Position;
	ChunkID chunkID;

	bool isEmpty = true;

	ChunkContainer* Neighbors[6]{nullptr};
	std::vector<SetBlockRelative> OutsideBlockToPlace[6]{};

	
private:
	bool InUse = false;
	BlockContainer Blocks;
};

#endif