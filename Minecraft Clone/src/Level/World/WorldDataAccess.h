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

	BlockID getBlock(int x, int y, int z);
	
	void setBlock(BlockID block, int x, int y, int z);

	void setChunk(Chunk* chunk);

	Chunk* getChunk(int x, int y, int z);

	Chunk* getChunk(glm::ivec3 pos);

	Chunk* getChunk(ChunkID ID);

	bool checkChunk(int x, int y, int z);

	bool checkChunk(ChunkID ID);
	
	bool checkChunk(glm::ivec3 v);

	std::vector<int> getColumnHeightmap(glm::ivec3 v) {
		ChunkColumn* col = ChunkContainer.GetColumn(v.x, v.y, v.z);
		if (col == nullptr) {
			return std::vector<int>(0);
		}
		return col->getHeightmap();
	}

	ChunkColumn* getColumn(glm::ivec3 v) {
		return ChunkContainer.GetColumn(v.x, v.y, v.z);
	}
};