#include "WorldDataAccess.h"

using namespace glm;
using namespace std;

BlockID WorldAccess::getBlock(int x, int y, int z) {
	return ChunkContainer.GetBlockGlobal(x, y, z);
}

void WorldAccess::setBlock(BlockID block, int x, int y, int z) {
	if (!ChunkContainer.SetBlockGlobal(block, x, y, z)) {
		throw exception("Tried to place block outside of the world");
	}
}

void WorldAccess::setChunk(Chunk* chunk) {
	ChunkContainer.InsertChunk(chunk);
}

Chunk* WorldAccess::getChunk(int x, int y, int z) {
	Chunk* chunk = ChunkContainer.GetChunk(x, y, z);

	return chunk;
}

Chunk* WorldAccess::getChunk(ivec3 v) {
	return getChunk(v.x, v.y, v.z);
}

Chunk* WorldAccess::getChunk(ChunkID ID) {
	return getChunk(ChunkIDToPOS(ID));
}

bool WorldAccess::checkChunk(int x, int y, int z) {
	return ChunkContainer.GetChunk(x, y, z) != nullptr;
}

bool WorldAccess::checkChunk(glm::ivec3 v) {
	return checkChunk(v.x, v.y, v.z);
}

bool WorldAccess::checkChunk(ChunkID id) {
	return checkChunk(ChunkIDToPOS(id));
}

Heightmap& WorldAccess::getColumnHeightmap(ivec3 v) {
	ChunkColumn* col = ChunkContainer.GetColumn(v.x, v.y, v.z);
	return col->getHeightmap();
}

ChunkColumn* WorldAccess::getColumn(ivec3 v) {
	return ChunkContainer.GetColumn(v.x, v.y, v.z);
}