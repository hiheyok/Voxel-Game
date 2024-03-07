#pragma once
#include "../DataContainer/BlockData.h"
#include "../TerrainGeneration/Structures/Structure.h"
#include "../Typenames.h"
#include <glm/vec3.hpp>
#include "ChunkID.h"
#include <atomic>

#ifndef CHUNKDATA_H
#define CHUNKDATA_H






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

	uint8_t X_block[16]{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	uint8_t Y_block[16]{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	uint8_t Z_block[16]{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	uint8_t TX_block[16]{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	uint8_t TY_block[16]{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	uint8_t TZ_block[16]{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	BlockContainer BlockStorage;
private:
	bool InUse = false;
	
};

#endif