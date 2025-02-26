#include "WorldDataAccess.h"

using namespace glm;
using namespace std;

BlockID WorldAccess::getBlock(const BlockPos& pos) {
	return ChunkContainer.GetBlockGlobal(pos);
}

void WorldAccess::setBlock(BlockID block, const BlockPos& pos) {
	if (!ChunkContainer.SetBlockGlobal(block, pos)) {
		throw exception("Tried to place block outside of the world");
	}
}

void WorldAccess::setChunk(Chunk* chunk) {
	ChunkContainer.InsertChunk(chunk);
}

Chunk* WorldAccess::getChunk(const ChunkPos& pos)  const {
	Chunk* chunk = ChunkContainer.GetChunk(pos);

	return chunk;
}

bool WorldAccess::checkChunk(const ChunkPos& id)  const {
	return  ChunkContainer.GetChunk(id) != nullptr;
}

Heightmap& WorldAccess::getColumnHeightmap(const ChunkColumnPos& v) const {
	ChunkColumn* col = ChunkContainer.GetColumn(v);
	return col->getHeightmap();
}

ChunkColumn* WorldAccess::getColumn(const ChunkPos& v) const {
	return ChunkContainer.GetColumn(v);
}