#include "WorldData.h"

using namespace std;
using namespace glm;

bool WorldData::SetBlock(BlockID block, int x, int y, int z) {

	int c[3]{ (int)floor((float)x / 16.f) ,(int)floor((float)y / 16.f) ,(int)floor((float)z / 16.f) }; //Location of chunk

	if (WorldChunkData.CheckChunk(c[0],c[1], c[2])) {

		WorldChunkData.SetBlockGlobal(block, x, y, z);
		ChunkUpdate(c[0], c[1], c[2]);

		for (int axis = 0; axis < 3; axis++) {
			for (int face = 0; face < 2; face++) {
				int p[3]{ c[0] ,c[1] ,c[2]}; // p[3] is just the position of the neighboring chunk

				p[axis] += (-2 * face) + 1;

				if (CheckChunk(p[0], p[1], p[2])) {
					ChunkUpdate(p[0], p[1], p[2]);
				}
			}
		}

		return true;
	}
	return false;
}

BlockID WorldData::GetBlock(int x, int y, int z) {
	return WorldChunkData.GetBlockGlobal(x, y, z);
}

void WorldData::SetChunk(Chunk chunk) {
	WorldChunkData.InsertChunk(chunk);
	ChunkUpdate(chunk.Position.x, chunk.Position.y, chunk.Position.z);
}

bool WorldData::EraseChunk(int x, int y, int z) {
	return WorldChunkData.EraseChunk(x, y, z);
}

bool WorldData::CheckChunk(int x, int y, int z) {
	return WorldChunkData.CheckChunk(x, y, z);
}

bool WorldData::CheckChunk(ChunkID id) {

	ivec3 pos = ChunkIDToPOS(id);

	return CheckChunk(pos.x, pos.y, pos.z);
}

Chunk& WorldData::GetChunk(ChunkID id) {
	return WorldChunkData[id];
}

Chunk& WorldData::GetChunk(int x, int y, int z) {
	return GetChunk(getChunkID(x, y, z));
}

void WorldData::ChunkUpdate(int x, int y, int z) {
	ChunkUpdateLock.lock();
	ChunksUpdated.insert(getChunkID(x, y, z));
	ChunkUpdateLock.unlock();
}

Chunk WorldData::GetCopyOfChunk(int x, int y, int z) {
	return WorldChunkData.GetCopyOfChunk(x, y, z);
}