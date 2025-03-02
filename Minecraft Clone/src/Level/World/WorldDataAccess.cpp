#include "WorldDataAccess.h"

using namespace glm;
using namespace std;

BlockID WorldAccess::GetBlock(const BlockPos& pos) {
	return chunk_container_.GetBlockGlobal(pos);
}

void WorldAccess::SetBlock(BlockID block, const BlockPos& pos) {
	if (!chunk_container_.SetBlockGlobal(block, pos)) {
		throw exception("Tried to place block outside of the world");
	}
}

void WorldAccess::SetChunk(Chunk* chunk) {
	chunk_container_.InsertChunk(chunk);
}

Chunk* WorldAccess::GetChunk(const ChunkPos& pos) const {
	return chunk_container_.GetChunk(pos);
}

bool WorldAccess::CheckChunk(const ChunkPos& pos) const {
	return chunk_container_.GetChunk(pos) != nullptr;
}

Heightmap& WorldAccess::GetColumnHeightmap(const ChunkColumnPos& v) const {
	ChunkColumn* col = chunk_container_.GetColumn(v);
	return col->GetHeightmap();
}

ChunkColumn* WorldAccess::GetColumn(const ChunkPos& v) const {
	return chunk_container_.GetColumn(v);
}