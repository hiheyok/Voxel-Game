#include "WorldData.h"

bool WorldData::SetBlock(BlockID block, int x, int y, int z) {
	return WorldChunkData.SetBlockGlobal(block, x, y, z);
}

BlockID WorldData::GetBlock(int x, int y, int z) {
	return WorldChunkData.GetBlockGlobal(x, y, z);
}

void WorldData::SetChunk(Chunk chunk) {
	WorldChunkData.InsertChunk(chunk);
}

bool WorldData::EraseChunk(int x, int y, int z) {
	WorldChunkData.EraseChunk(x, y, z);
}

bool WorldData::CheckChunk(int x, int y, int z) {
	return WorldChunkData.CheckChunk(x, y, z);
}