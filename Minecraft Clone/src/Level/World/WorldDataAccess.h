#pragma once
#include "../DataContainer/ChunkMapData.h"
#include "../DataContainer/EntityContainer.h"
#include <exception>
#include <glm/vec3.hpp>

class WorldAccess {
private:
	ChunkMap ChunkContainer;
public:
	EntityContainer Entities; //Remove tick later, this is going to be specific to dimension stuff

	BlockID getBlock(const BlockPos& pos);
	
	void setBlock(BlockID block, const BlockPos& pos);

	void setChunk(Chunk* chunk);

	Chunk* getChunk(const ChunkPos& ID) const;

	bool checkChunk(const ChunkPos& ID) const;
	
	Heightmap& getColumnHeightmap(const ChunkColumnPos& v) const;

	ChunkColumn* getColumn(const ChunkPos& v) const;
};