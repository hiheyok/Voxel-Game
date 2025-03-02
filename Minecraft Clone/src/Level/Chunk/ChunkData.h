#pragma once

#ifndef CHUNKDATA_H
#define CHUNKDATA_H

#include <glm/vec3.hpp>

#include "../DataContainer/BlockData.h"
#include "../TerrainGeneration/Structures/Structure.h"
#include "Lighting/ChunkLighting.h"
#include "../Typenames.h"
#include "ChunkPos/ChunkPos.h"



class chunk_container_ {
	
public:

	chunk_container_() {
		lighting_ = std::make_shared<ChunkLightingContainer>();
	}
	
	void SetNeighbor(chunk_container_* neighbor, unsigned int side);

	void ClearNeighbors();

	BlockID GetBlock(glm::ivec3 posIn) const;
	BlockID GetBlockUnsafe(glm::ivec3 posIn) const;

	BlockID GetBlock(int x, int y, int z) const;
	BlockID GetBlockUnsafe(int x, int y, int z) const;

	void SetBlock(BlockID block, int x, int y, int z);
	void SetBlockUnsafe(BlockID block, int x, int y, int z);

	void Use();
	void Unuse();

	chunk_container_* GetNeighbor(unsigned int side) const;
	
	void SetPosition(int x, int y, int z);

	ChunkPos position_;

	bool is_empty_ = true;

	chunk_container_* neighbors_[6]{nullptr};
	std::vector<SetBlockRelative> outside_block_to_place_[6]{};
	std::shared_ptr<ChunkLightingContainer> lighting_;

	BlockContainer block_storage_;
private:
	bool in_use_ = false;
	
};

#endif