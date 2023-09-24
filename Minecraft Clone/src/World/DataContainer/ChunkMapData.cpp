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

	int ChunkPos[3]{ (int)floor((float)x / 16.f) ,(int)floor((float)y / 16.f) ,(int)floor((float)z / 16.f) };
	int LocalBlockPos[3]{ x - ChunkPos[0] * 16 ,y - ChunkPos[1] * 16,z - ChunkPos[2] * 16 };

	Region* reg = GetRegion(ChunkPos[0], ChunkPos[1], ChunkPos[2]);

	if (reg != nullptr) {
		uint16_t cx = abs(ChunkPos[0] % 32);
		uint16_t cy = abs(ChunkPos[1] % 32);
		uint16_t cz = abs(ChunkPos[2] % 32);

		Chunk* chunk = reg->GetChunk(cx, cy, cz);

		if (chunk != nullptr) {
			chunk->Use();
			chunk->SetBlockUnsafe(block, LocalBlockPos[0], LocalBlockPos[1], LocalBlockPos[2]);
			chunk->isEmpty = false;
			chunk->Unuse();
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

	int c[3]{ (int)floor((float)x / 16.f) ,(int)floor((float)y / 16.f) ,(int)floor((float)z / 16.f) };

	int l[3]{ x - c[0] * 16 ,y - c[1] * 16,z - c[2] * 16 };

	int r[3]{ (int)floor((float)x / 512.f) ,(int)floor((float)y / 512.f) ,(int)floor((float)z / 512.f) };

	Region* reg = GetRegion(c[0], c[1], c[2]);
	if (reg == nullptr) {
		return AIR;
	}

	Chunk* chunk = reg->GetChunkGlobalPos(c[0], c[1], c[2]);

	if (chunk == nullptr) {
		return AIR;
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

void ChunkMap::InsertChunk(Chunk* chunk) {
	int x = chunk->Position.x;
	int y = chunk->Position.y;
	int z = chunk->Position.z;

	Region* reg = GetRegion(x, y, z);

	int RegCX = abs(x % 32);
	int RegCY = abs(y % 32);
	int RegCZ = abs(z % 32);

	int rx = (int)floor((float)x / 32.f);
	int ry = (int)floor((float)y / 32.f);
	int rz = (int)floor((float)z / 32.f);

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
	int rx = (int)floor((float)x / 32.f);
	int ry = (int)floor((float)y / 32.f);
	int rz = (int)floor((float)z / 32.f);

	RegionID regID = getChunkID(rx, ry, rz);

	if (!LiveRegion.count(regID)) {
		return nullptr;
	}
	return &LiveRegion[regID];
}
//_____________________________________________________

void Region::AddChunk(Chunk* chunk, uint16_t x, uint16_t y, uint16_t z) {
	region[x * 1024 + y * 32 + z] = chunk;
}

void Region::EraseChunk(uint16_t x, uint16_t y, uint16_t z) {
	Chunk* c = region[x * 1024 + y * 32 + z];
	if (c == nullptr) {
		return;
	}

	c->ClearNeighbors();

	delete c;
	region[x * 1024 + y * 32 + z] = nullptr;
}

bool Region::CheckChunk(uint16_t x, uint16_t y, uint16_t z) {
	return region[x * 1024 + y * 32 + z] != nullptr;
}

Chunk* Region::GetChunk(uint16_t x, uint16_t y, uint16_t z) {
	return region[x * 1024 + y * 32 + z];
}

void Region::AddChunkGlobalPos(Chunk* chunk, int32_t x, int32_t y, int32_t z) {
	uint16_t RegCX = (uint16_t)abs(x % 32);
	uint16_t RegCY = (uint16_t)abs(y % 32);
	uint16_t RegCZ = (uint16_t)abs(z % 32);

	AddChunk(chunk, RegCX, RegCY, RegCZ);
}

void Region::EraseChunkGlobalPos(int32_t x, int32_t y, int32_t z) {
	uint16_t RegCX = (uint16_t)abs(x % 32);
	uint16_t RegCY = (uint16_t)abs(y % 32);
	uint16_t RegCZ = (uint16_t)abs(z % 32);

	EraseChunk(RegCX, RegCY, RegCZ);
}

bool Region::CheckChunkGlobalPos(int32_t x, int32_t y, int32_t z) {
	uint16_t RegCX = (uint16_t)abs(x % 32);
	uint16_t RegCY = (uint16_t)abs(y % 32);
	uint16_t RegCZ = (uint16_t)abs(z % 32);

	return GetChunk(RegCX, RegCY, RegCZ);
}

Chunk* Region::GetChunkGlobalPos(int32_t x, int32_t y, int32_t z) {
	uint16_t RegCX = (uint16_t)abs(x % 32);
	uint16_t RegCY = (uint16_t)abs(y % 32);
	uint16_t RegCZ = (uint16_t)abs(z % 32);

	return GetChunk(RegCX, RegCY, RegCZ);
}