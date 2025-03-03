#include "ChunkMapData.h"


bool ChunkMap::EraseChunk(const ChunkPos& pos) { 
	const std::unique_ptr<Region>& reg = GetRegion(pos);

	// TODO: Fix me

	if (reg != nullptr) {
		reg->EraseChunkGlobalPos(pos.x, pos.y, pos.z);
		return true;
	}
	return false;
}


bool ChunkMap::SetBlockGlobal(BlockID block, const BlockPos& pos) {
	// TODO: Make it so BlockPos has a method to convert to chunk position location
	ChunkPos chunkPos( pos.x >> 4, pos.y >> 4, pos.z >> 4);
	int localBlockPos[3]{ pos.x & 0b1111, pos.y & 0b1111, pos.z & 0b1111};

	if (!CheckRegion(chunkPos)) return false;

	const std::unique_ptr<Region>& reg = GetRegion(chunkPos);

	uint16_t cx = chunkPos[0] & 0b11111;
	uint16_t cy = chunkPos[1] & 0b11111;
	uint16_t cz = chunkPos[2] & 0b11111;

	Chunk* chunk = reg->GetChunk(cx, cy, cz);

	if (chunk != nullptr) {
		chunk->Use();
		chunk->SetBlockUnsafe(block, localBlockPos[0], localBlockPos[1], localBlockPos[2]);
		chunk->is_empty_ = false;
		chunk->Unuse();

		//Set lighting update to dirty
			
		ChunkColumn* col = reg->GetChunkColumn(cx, cz);
		col->UpdateHeightmapSingleBlock(chunkPos[1] & 0b11111, block, localBlockPos[0], localBlockPos[1], localBlockPos[2]);

		return true;
	}

	return false;
}

bool ChunkMap::CheckChunk(const ChunkPos& pos) const {
	if (!CheckRegion(pos)) return g_blocks.AIR;
	const std::unique_ptr<Region>& reg = GetRegion(pos);

	return reg->CheckChunkGlobalPos(pos.x, pos.y, pos.z);
}

BlockID ChunkMap::GetBlockGlobal(const BlockPos& pos) {
	//c[3] is the position of the chunk
	//l[3] is the local position of a block inside a chunk

	int x = pos.x;
	int y = pos.y;
	int z = pos.z;

	ChunkPos c{ x >> 4 ,y >> 4 ,z >> 4 };

	if (!CheckRegion(c)) return g_blocks.AIR;

	int l[3]{ x & 0b1111 ,y & 0b1111,z & 0b1111 };

	ChunkPos r{ x >> 9 ,y >> 9 ,z >> 9 }; // region 

	const std::unique_ptr<Region>& reg = GetRegion(c);

	// TODO: Fix me
	Chunk* chunk = reg->GetChunkGlobalPos(c[0], c[1], c[2]);

	if (chunk == nullptr) {
		return g_blocks.AIR;
	}

	BlockID block = chunk->GetBlock(l[0], l[1], l[2]);

	return block;
}

Chunk* ChunkMap::GetChunk(const ChunkPos& pos) const {
	if (!CheckRegion(pos)) return nullptr;
	const std::unique_ptr<Region>& reg = GetRegion(pos);

	return reg->GetChunkGlobalPos(pos.x, pos.y, pos.z);
}

ChunkColumn* ChunkMap::GetColumn(const ChunkColumnPos& pos) const {
	if (!CheckRegion(pos)) return nullptr;
	const std::unique_ptr<Region>& reg = GetRegion(pos);

	return reg->GetChunkColumnGlobalPos(pos.x, pos.z);
}

void ChunkMap::InsertChunk(Chunk* chunk) {
	const ChunkPos& pos = chunk->position_;
	
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;


	int rx = x >> 5;
	int ry = y >> 5;
	int rz = z >> 5;

	if (!CheckRegion(pos)) {
		live_region_[RegionPos{rx, ry, rz}] = std::make_unique<Region>();
	}

	const std::unique_ptr<Region>& reg = GetRegion(pos);

	reg->AddChunkGlobalPos(chunk, x, y, z);

	for (int axis_ = 0; axis_ < 3; axis_++) {
		for (int face = 0; face < 2; face++) {
			ChunkPos posNeighbor = chunk->position_;
			posNeighbor.incrementSide(axis_ * 2 + face, 1);
			
			if (CheckChunk(posNeighbor)) {
				chunk->SetNeighbor(static_cast<chunk_container_*>(GetChunk(posNeighbor)), axis_ * 2 + face);
				GetChunk(posNeighbor)->SetNeighbor(static_cast<chunk_container_*>(chunk), axis_ * 2 - face + 1);
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

const bool ChunkMap::CheckRegion(const ChunkPos& pos) const {
	int rx = pos.x >> 5;
	int ry = pos.y >> 5;
	int rz = pos.z >> 5;

	// TODO: Use a different class for region pos 
	RegionPos regPos{ rx, ry, rz };

	FastHashMap<RegionPos, std::unique_ptr<Region>>::const_iterator it = live_region_.find(regPos);

	if (it == live_region_.end()) {
		return false;
	}
	else {
		return true;
	}
}

const std::unique_ptr<Region>& ChunkMap::GetRegion(const ChunkPos& pos) const {
	int rx = pos.x >> 5;
	int ry = pos.y >> 5;
	int rz = pos.z >> 5;

	// TODO: Use a different class for region pos 
	RegionPos regPos{ rx, ry, rz };

	FastHashMap<RegionPos, std::unique_ptr<Region>>::const_iterator it = live_region_.find(regPos);

	if (it == live_region_.end()) {
		throw std::runtime_error("Tried to get invalid region");
	}

	return it->second;
}
//_____________________________________________________

void Region::AddChunk(Chunk* chunk, uint16_t x, uint16_t y, uint16_t z) {
	if (region_[x * 32 + z] == nullptr) {
		region_[x * 32 + z] = new ChunkColumn();
	}
	region_[x * 32 + z]->AddChunk(chunk, y);
}

void Region::EraseChunk(uint16_t x, uint16_t y, uint16_t z) {
	ChunkColumn* c = region_[x * 32 + z];
	if (c == nullptr) return;
	if (c->GetChunk(y) == nullptr) return;

	c->GetChunk(y)->ClearNeighbors();

	delete c->GetChunk(y);
	region_[x * 32 + z] = nullptr;
}

bool Region::CheckChunk(uint16_t x, uint16_t y, uint16_t z) const {
	if (!CheckChunkColumn(x, z)) {
		return false;
	}
	return region_[x * 32 + z]->GetChunk(y) != nullptr;
}

Chunk* Region::GetChunk(uint16_t x, uint16_t y, uint16_t z) const {
	if (region_[x * 32 + z] != nullptr) {
		return region_[x * 32 + z]->GetChunk(y);
	}
	return nullptr;
}

void Region::AddChunkGlobalPos(Chunk* chunk, int32_t x, int32_t y, int32_t z) {
	uint16_t regCX = x & 0b11111;
	uint16_t regCY = y & 0b11111;
	uint16_t regCZ = z & 0b11111;

	AddChunk(chunk, regCX, regCY, regCZ);
}

void Region::EraseChunkGlobalPos(int32_t x, int32_t y, int32_t z) {
	uint16_t regCX = x & 0b11111;
	uint16_t regCY = y & 0b11111;
	uint16_t regCZ = z & 0b11111;

	EraseChunk(regCX, regCY, regCZ);
}

bool Region::CheckChunkGlobalPos(int32_t x, int32_t y, int32_t z) const {
	uint16_t regCX = x & 0b11111;
	uint16_t regCY = y & 0b11111;
	uint16_t regCZ = z & 0b11111;

	return CheckChunk(regCX, regCY, regCZ);
}

Chunk* Region::GetChunkGlobalPos(int32_t x, int32_t y, int32_t z) const {
	uint16_t regCX = x & 0b11111;
	uint16_t regCY = y & 0b11111;
	uint16_t regCZ = z & 0b11111;

	return GetChunk(regCX, regCY, regCZ);
}

ChunkColumn* Region::GetChunkColumn(uint16_t x, uint16_t z) const {
	return region_[x * 32 + z];
}

ChunkColumn* Region::GetChunkColumnGlobalPos(int32_t x, int32_t z) const {
	uint16_t regCX = x & 0b11111;
	uint16_t regCZ = z & 0b11111;

	return GetChunkColumn(regCX, regCZ);
}

bool Region::CheckChunkColumn(uint16_t x, uint16_t z) const {
	return region_[x * 32 + z] != nullptr;
}
bool Region::CheckChunkColumnGlobalPos(int32_t x, int32_t z) const {
	uint16_t regCX = x & 0b11111;
	uint16_t regCZ = z & 0b11111;

	return CheckChunkColumn(regCX, regCZ);
}