#pragma once
#include "../DataContainer/BlockData.h"
#include "../TerrainGeneration/Structures/Structure.h"
#include "Lighting/ChunkLighting.h"
#include "../Typenames.h"
#include <glm/vec3.hpp>
#include "ChunkID.h"

#ifndef CHUNKDATA_H
#define CHUNKDATA_H

class ChunkContainer {
	
public:
	
	void SetNeighbor(ChunkContainer* Neighbor, unsigned int Side);

	void ClearNeighbors();

	BlockID GetBlock(glm::ivec3 posIn);
	BlockID GetBlockUnsafe(glm::ivec3 posIn);

	BlockID GetBlock(int x, int y, int z);
	BlockID GetBlockUnsafe(int x, int y, int z);

	void SetBlock(BlockID block, int x, int y, int z);
	void SetBlockUnsafe(BlockID block, int x, int y, int z);

	void Use();

	void Unuse();

	ChunkContainer* GetNeighbor(unsigned int Side);
	
	void SetPosition(int x, int y, int z);

	glm::ivec3 Position;
	ChunkID chunkID;

	bool isEmpty = true;

	ChunkContainer* Neighbors[6]{nullptr};
	std::vector<SetBlockRelative> OutsideBlockToPlace[6]{};
	ChunkLightingContainer Lighting;

	BlockContainer BlockStorage;
private:
	bool InUse = false;
	
};

#endif