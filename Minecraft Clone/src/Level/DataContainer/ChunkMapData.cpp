#include "ChunkMapData.h"


bool ChunkMap::EraseChunk(const ChunkPos& pos) { 
	Region* reg = GetRegion(pos);

	// TODO: Fix me

	if (reg != nullptr) {
		reg->EraseChunkGlobalPos(pos.x, pos.y, pos.z);
		return true;
	}
	return false;
}


bool ChunkMap::SetBlockGlobal(BlockID block, const BlockPos& pos) {
	// TODO: Make it so BlockPos has a method to convert to chunk position location
	ChunkPos ChunkPos( pos.x >> 4, pos.y >> 4, pos.z >> 4);
	int LocalBlockPos[3]{ pos.x & 0b1111, pos.y & 0b1111, pos.z & 0b1111};

	Region* reg = GetRegion(ChunkPos);

	if (reg != nullptr) {
		uint16_t cx = ChunkPos[0] & 0b11111;
		uint16_t cy = ChunkPos[1] & 0b11111;
		uint16_t cz = ChunkPos[2] & 0b11111;

		Chunk* chunk = reg->GetChunk(cx, cy, cz);

		if (chunk != nullptr) {
			chunk->Use();
			chunk->SetBlockUnsafe(block, LocalBlockPos[0], LocalBlockPos[1], LocalBlockPos[2]);
			chunk->isEmpty = false;
			chunk->Unuse();

			//Set lighting update to dirty
			
			ChunkColumn* col = reg->GetChunkColumn(cx, cz);
			col->UpdateHeightmapSingleBlock(ChunkPos[1] & 0b11111, block, LocalBlockPos[0], LocalBlockPos[1], LocalBlockPos[2]);

			return true;
		}

		return false;
	}
	return false;
}

bool ChunkMap::CheckChunk(const ChunkPos& pos) const {
	Region* reg = GetRegion(pos);

	// TODO: Fix me

	if (reg != nullptr) {
		return reg->CheckChunkGlobalPos(pos.x, pos.y, pos.z);
	}
	return false;
}

BlockID ChunkMap::GetBlockGlobal(const BlockPos& pos) {
	//c[3] is the position of the chunk
	//l[3] is the local position of a block inside a chunk

	int x = pos.x;
	int y = pos.y;
	int z = pos.z;

	ChunkPos c{ x >> 4 ,y >> 4 ,z >> 4 };

	int l[3]{ x & 0b1111 ,y & 0b1111,z & 0b1111 };

	ChunkPos r{ x >> 9 ,y >> 9 ,z >> 9 }; // region 

	Region* reg = GetRegion(c);
	if (reg == nullptr) {
		return Blocks.AIR;
	}
	// TODO: Fix me
	Chunk* chunk = reg->GetChunkGlobalPos(c[0], c[1], c[2]);

	if (chunk == nullptr) {
		return Blocks.AIR;
	}

	BlockID block = chunk->GetBlock(l[0], l[1], l[2]);

	return block;
}

Chunk* ChunkMap::GetChunk(const ChunkPos& pos) const {
	Region* reg = GetRegion(pos);

	// TODO: Fix me
	if (reg != nullptr) {
		return reg->GetChunkGlobalPos(pos.x, pos.y, pos.z);
	}
	return nullptr;
}

ChunkColumn* ChunkMap::GetColumn(const ChunkColumnPos& pos) const {
	Region* reg = GetRegion(pos);

	if (reg != nullptr) {
		return reg->GetChunkColumnGlobalPos(pos.x, pos.z);
	}
	return nullptr;
}

void ChunkMap::InsertChunk(Chunk* chunk) {
	const ChunkPos& pos = chunk->position_;
	
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;

	Region* reg = GetRegion(pos);

	int rx = x >> 5;
	int ry = y >> 5;
	int rz = z >> 5;

	if (reg == nullptr) {
		Logger.LogDebug("ChunkMap", "Created new region");
		reg = new Region;
		LiveRegion[RegionPos{rx, ry, rz}] = reg;
	}
	
	reg->AddChunkGlobalPos(chunk, x, y, z);

	for (int axis = 0; axis < 3; axis++) {
		for (int face = 0; face < 2; face++) {
			ChunkPos posNeighbor = chunk->position_;
			posNeighbor.incrementSide(axis * 2 + face, 1);
			
			if (CheckChunk(posNeighbor)) {
				chunk->SetNeighbor(static_cast<ChunkContainer*>(GetChunk(posNeighbor)), axis * 2 + face);
				GetChunk(posNeighbor)->SetNeighbor(static_cast<ChunkContainer*>(chunk), axis * 2 - face + 1);
				GetChunk(posNeighbor)->Use();
				GetChunk(posNeighbor)->UpdateGen();
				GetChunk(posNeighbor)->Unuse();
			}
		}
	}
	chunk->Use();
	chunk->UpdateGen();
	chunk->Unuse();
}

Region* ChunkMap::GetRegion(const ChunkPos& pos) const {
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;

	int rx = x >> 5;
	int ry = y >> 5;
	int rz = z >> 5;

	// TODO: Use a different class for region pos 
	RegionPos regPos{ rx, ry, rz };

	FastHashMap<RegionPos, Region*>::const_iterator it = LiveRegion.find(regPos);

	if (it == LiveRegion.end()) {
		return nullptr;
	}

	return it->second;
}
//_____________________________________________________

void Region::AddChunk(Chunk* chunk, uint16_t x, uint16_t y, uint16_t z) {
	if (region[x * 32 + z] == nullptr) {
		region[x * 32 + z] = new ChunkColumn();
	}
	region[x * 32 + z]->AddChunk(chunk, y);
}

void Region::EraseChunk(uint16_t x, uint16_t y, uint16_t z) {
	ChunkColumn* c = region[x * 32 + z];
	if (c == nullptr) return;
	if (c->GetChunk(y) == nullptr) return;

	c->GetChunk(y)->ClearNeighbors();

	delete c->GetChunk(y);
	region[x * 32 + z] = nullptr;
}

bool Region::CheckChunk(uint16_t x, uint16_t y, uint16_t z) const {
	if (!CheckChunkColumn(x, z)) {
		return false;
	}
	return region[x * 32 + z]->GetChunk(y) != nullptr;
}

Chunk* Region::GetChunk(uint16_t x, uint16_t y, uint16_t z) const {
	if (region[x * 32 + z] != nullptr) {
		return region[x * 32 + z]->GetChunk(y);
	}
	return nullptr;
}

void Region::AddChunkGlobalPos(Chunk* chunk, int32_t x, int32_t y, int32_t z) {
	uint16_t RegCX = x & 0b11111;
	uint16_t RegCY = y & 0b11111;
	uint16_t RegCZ = z & 0b11111;

	AddChunk(chunk, RegCX, RegCY, RegCZ);
}

void Region::EraseChunkGlobalPos(int32_t x, int32_t y, int32_t z) {
	uint16_t RegCX = x & 0b11111;
	uint16_t RegCY = y & 0b11111;
	uint16_t RegCZ = z & 0b11111;

	EraseChunk(RegCX, RegCY, RegCZ);
}

bool Region::CheckChunkGlobalPos(int32_t x, int32_t y, int32_t z) const {
	uint16_t RegCX = x & 0b11111;
	uint16_t RegCY = y & 0b11111;
	uint16_t RegCZ = z & 0b11111;

	return CheckChunk(RegCX, RegCY, RegCZ);
}

Chunk* Region::GetChunkGlobalPos(int32_t x, int32_t y, int32_t z) const {
	uint16_t RegCX = x & 0b11111;
	uint16_t RegCY = y & 0b11111;
	uint16_t RegCZ = z & 0b11111;

	return GetChunk(RegCX, RegCY, RegCZ);
}

ChunkColumn* Region::GetChunkColumn(uint16_t x, uint16_t z) const {
	return region[x * 32 + z];
}

ChunkColumn* Region::GetChunkColumnGlobalPos(int32_t x, int32_t z) const {
	uint16_t RegCX = x & 0b11111;
	uint16_t RegCZ = z & 0b11111;

	return GetChunkColumn(RegCX, RegCZ);
}

bool Region::CheckChunkColumn(uint16_t x, uint16_t z) const {
	return region[x * 32 + z] != nullptr;
}
bool Region::CheckChunkColumnGlobalPos(int32_t x, int32_t z) const {
	uint16_t RegCX = x & 0b11111;
	uint16_t RegCZ = z & 0b11111;

	return CheckChunkColumn(RegCX, RegCZ);
}