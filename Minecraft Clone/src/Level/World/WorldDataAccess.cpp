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

	if (chunk == nullptr) {
		throw exception(string("Cannot find chunk at: " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z)).c_str());
	}

	return chunk;
}

Chunk* WorldAccess::getChunk(ivec3 v) {
	return getChunk(v.x, v.y, v.z);
}

Chunk* WorldAccess::getChunk(ChunkID ID) {
	return getChunk(ChunkIDToPOS(ID));
}