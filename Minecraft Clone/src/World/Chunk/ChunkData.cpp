#include "ChunkData.h"

BlockID ChunkContainer::GetBlock(int x, int y, int z) {
	if (x >= 16 || y >= 16 || z >= 16 || x < 0 || y < 0 || z < 0) {
		if (x < 0 && Neighbors[NX] != nullptr) {
			return Neighbors[NX]->GetBlock(x + 16, y, z);
		}

		if (x >= 16 && Neighbors[PX] != nullptr) {
			return Neighbors[PX]->GetBlock(x - 16, y, z);
		}

		if (y < 0 && Neighbors[NY] != nullptr) {
			return Neighbors[NY]->GetBlock(x, y + 16, z);
		}

		if (y >= 16 && Neighbors[PY] != nullptr) {
			return Neighbors[PY]->GetBlock(x, y - 16, z);
		}

		if (z < 0 && Neighbors[NZ] != nullptr) {
			return Neighbors[NZ]->GetBlock(x, y, z + 16);
		}

		if (z >= 16 && Neighbors[PZ] != nullptr) {
			return Neighbors[PZ]->GetBlock(x, y, z - 16);
		}

		return Blocks.AIR;
	}
	else {
		return BlockStorage.GetBlock(x, y, z);
	}
}

BlockID ChunkContainer::GetBlockUnsafe(int x, int y, int z) {
	return BlockStorage.GetBlock(x, y, z);
}

void ChunkContainer::SetBlock(BlockID block, int x, int y, int z) {
	if (x >= 16 || y >= 16 || z >= 16 || x < 0 || y < 0 || z < 0) {
		if (x < 0) {
			OutsideBlockToPlace[NX].emplace_back(block, x + 16, y, z);
			return;
		}

		if (x >= 16) {
			OutsideBlockToPlace[PX].emplace_back(block, x - 16, y, z);
			return;
		}

		if (y < 0) {
			OutsideBlockToPlace[NY].emplace_back(block, x, y + 16, z);
			return;
		}

		if (y >= 16) {
			OutsideBlockToPlace[PY].emplace_back(block, x, y - 16, z);
			return;
		}

		if (z < 0) {
			OutsideBlockToPlace[NZ].emplace_back(block, x, y, z + 16);
			return;
		}

		if (z >= 16) {
			OutsideBlockToPlace[PZ].emplace_back(block, x, y, z - 16);
			return;
		}
	}
	else {
		int16_t count = (block != Blocks.AIR) * (BlockStorage.GetBlock(x, y, z) == Blocks.AIR) - (block == Blocks.AIR);

		BlockStorage.ChangeBlock(block, (uint32_t)x, (uint32_t)y, (uint32_t)z);
		isEmpty = false;

		X_block[x] += count;
		Y_block[y] += count;
		Z_block[z] += count;
	}
}

void ChunkContainer::SetBlockUnsafe(BlockID block, int x, int y, int z) {
	int16_t count = (block != Blocks.AIR) * (BlockStorage.GetBlock(x, y, z) == Blocks.AIR) - (block == Blocks.AIR);

	BlockStorage.ChangeBlock(block, (uint32_t)x, (uint32_t)y, (uint32_t)z);

	X_block[x] += count;
	Y_block[y] += count;
	Z_block[z] += count;

	isEmpty = false;
}

void ChunkContainer::SetPosition(int x, int y, int z) {
	Position.x = x;
	Position.y = y;
	Position.z = z;
	chunkID = getChunkID(x, y, z);
}

inline ChunkContainer* ChunkContainer::GetNeighbor(unsigned int Side) {
	return Neighbors[Side];
}

void ChunkContainer::SetNeighbor(ChunkContainer* Neighbor, unsigned int Side) {
	Neighbors[Side] = Neighbor;
}

void ChunkContainer::ClearNeighbors() {
	for (int i = 0; i < 6; i++) {
		Neighbors[i] = nullptr;
	}
}

void ChunkContainer::Use() {
	while (InUse) {};
	InUse = true;
}

void ChunkContainer::Unuse() {
	InUse = false;
}