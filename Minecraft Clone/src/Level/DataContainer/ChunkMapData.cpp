#include "ChunkMapData.h"


bool ChunkMap::EraseChunk(int x, int y, int z) { 
	Region* reg = GetRegion(x, y, z);

	if (reg != nullptr) {
		reg->EraseChunkGlobalPos(x, y, z);
		return true;
	}
	return false;
}


bool ChunkMap::SetBlockGlobal(BlockID block, int x, int y, int z) {

	int ChunkPos[3]{ x >> 4 ,y >> 4 ,z >> 4 };
	int LocalBlockPos[3]{ x & 0b1111, y & 0b1111, z & 0b1111 };

	Region* reg = GetRegion(ChunkPos[0], ChunkPos[1], ChunkPos[2]);

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
			col->UpdateHeightmap(ChunkPos[1] & 0b11111);
			return true;
		}

		

		return false;
	}
	return false;
}

bool ChunkMap::CheckChunk(int x, int y, int z) {
	Region* reg = GetRegion(x, y, z);

	if (reg != nullptr) {
		return reg->CheckChunkGlobalPos(x, y, z);
	}
	return false;
}

BlockID ChunkMap::GetBlockGlobal(int x, int y, int z) {
	//c[3] is the position of the chunk
	//l[3] is the local position of a block inside a chunk

	int c[3]{ x >> 4 ,y >> 4 ,z >> 4 };

	int l[3]{ x & 0b1111 ,y & 0b1111,z & 0b1111 };

	int r[3]{ x >> 9 ,y >> 9 ,z >> 9 }; // region 

	Region* reg = GetRegion(c[0], c[1], c[2]);
	if (reg == nullptr) {
		return Blocks.AIR;
	}

	Chunk* chunk = reg->GetChunkGlobalPos(c[0], c[1], c[2]);

	if (chunk == nullptr) {
		return Blocks.AIR;
	}

	BlockID block = chunk->GetBlock(l[0], l[1], l[2]);

	return block;
}

Chunk* ChunkMap::GetChunk(int x, int y, int z) {
	Region* reg = GetRegion(x, y, z);

	if (reg != nullptr) {
		return reg->GetChunkGlobalPos(x, y, z);
	}
	return nullptr;
}

ChunkColumn* ChunkMap::GetColumn(int x, int y, int z) {
	Region* reg = GetRegion(x, y, z);

	if (reg != nullptr) {
		return reg->GetChunkColumnGlobalPos(x, z);
	}
	return nullptr;
}

void ChunkMap::InsertChunk(Chunk* chunk) {
	int x = chunk->Position.x;
	int y = chunk->Position.y;
	int z = chunk->Position.z;

	Region* reg = GetRegion(x, y, z);

	int rx = x >> 5;
	int ry = y >> 5;
	int rz = z >> 5;

	if (reg == nullptr) {
		Region* newRegion = new Region;
		LiveRegion[getChunkID(rx, ry, rz)] = *newRegion;
	}
	reg = &LiveRegion[getChunkID(rx, ry, rz)];

	reg->AddChunkGlobalPos(chunk, x, y, z);

	for (int axis = 0; axis < 3; axis++) {
		for (int face = 0; face < 2; face++) {
			int p[3]{ x, y, z}; // p[3] is just the position of the neighboring chunk

			p[axis] += (-2 * face) + 1;

			if (CheckChunk(p[0], p[1], p[2])) {
				GetChunk(x, y, z)->SetNeighbor((ChunkContainer*)GetChunk(p[0], p[1], p[2]), axis * 2 + face);
				GetChunk(p[0], p[1], p[2])->SetNeighbor((ChunkContainer*)GetChunk(x, y, z), axis * 2 - face + 1);
				GetChunk(p[0], p[1], p[2])->Use();
				GetChunk(p[0], p[1], p[2])->UpdateGen();
				GetChunk(p[0], p[1], p[2])->Unuse();
			}
		}
	}
	chunk->Use();
	chunk->UpdateGen();
	chunk->Unuse();
}

Region* ChunkMap::GetRegion(int x, int y, int z) {
	int rx = x >> 5;
	int ry = y >> 5;
	int rz = z >> 5;

	RegionID regID = getChunkID(rx, ry, rz);

	std::unordered_map<RegionID, Region>::iterator it = LiveRegion.find(regID);

	if (it == LiveRegion.end()) {
		return nullptr;
	}

	return &it->second;
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

bool Region::CheckChunk(uint16_t x, uint16_t y, uint16_t z) {
	if (!CheckChunkColumn(x, z)) {
		return false;
	}
	return region[x * 32 + z]->GetChunk(y) != nullptr;
}

Chunk* Region::GetChunk(uint16_t x, uint16_t y, uint16_t z) {
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

bool Region::CheckChunkGlobalPos(int32_t x, int32_t y, int32_t z) {
	uint16_t RegCX = x & 0b11111;
	uint16_t RegCY = y & 0b11111;
	uint16_t RegCZ = z & 0b11111;

	return CheckChunk(RegCX, RegCY, RegCZ);
}

Chunk* Region::GetChunkGlobalPos(int32_t x, int32_t y, int32_t z) {
	uint16_t RegCX = x & 0b11111;
	uint16_t RegCY = y & 0b11111;
	uint16_t RegCZ = z & 0b11111;

	return GetChunk(RegCX, RegCY, RegCZ);
}

ChunkColumn* Region::GetChunkColumn(uint16_t x, uint16_t z) {
	return region[x * 32 + z];
}

ChunkColumn* Region::GetChunkColumnGlobalPos(int32_t x, int32_t z) {
	uint16_t RegCX = x & 0b11111;
	uint16_t RegCZ = z & 0b11111;

	return GetChunkColumn(RegCX, RegCZ);
}

bool Region::CheckChunkColumn(uint16_t x, uint16_t z) {
	return region[x * 32 + z] != nullptr;
}
bool Region::CheckChunkColumnGlobalPos(int32_t x, int32_t z) {
	uint16_t RegCX = x & 0b11111;
	uint16_t RegCZ = z & 0b11111;

	return CheckChunkColumn(RegCX, RegCZ);
}