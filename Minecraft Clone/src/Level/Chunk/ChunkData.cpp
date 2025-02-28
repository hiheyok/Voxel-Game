#include "ChunkData.h"

static const int NeighborOffset[2] = {
	  -16, 16
};

BlockID ChunkContainer::GetBlock(glm::ivec3 posIn) const {
	return GetBlock(posIn.x, posIn.y, posIn.z);
}

BlockID ChunkContainer::GetBlockUnsafe(glm::ivec3 posIn) const {
	return GetBlockUnsafe(posIn.x, posIn.y, posIn.z);
}

BlockID ChunkContainer::GetBlock(int x, int y, int z) const {
	if (!((x | y | z) >> 4)) { //check if it is in the chunk
		return BlockStorage.GetBlock(x, y, z);
	}

	int dx = (((x >> 31) & 0b1) + 1) & -(!!(x >> 4));
	int dy = (((y >> 31) & 0b1) + 2) & -(!!(y >> 4));
	int dz = (((z >> 31) & 0b1) + 4) & -(!!(z >> 4));

	//Set to 0 if neighbor is null
	dx &= -!!(Neighbors[dx - 1]);
	dy &= -!!(Neighbors[dy]);
	dz &= -!!(Neighbors[dz]);

	if (dx != 0) return Neighbors[dx - 1]->GetBlock(x + NeighborOffset[(dx - 1) & 1], y, z);
	if (dy != 0) return Neighbors[dy]->GetBlock(x, y + NeighborOffset[dy & 0b1 ], z);
	if (dz != 0) return Neighbors[dz]->GetBlock(x, y, z + NeighborOffset[dz & 0b1]);

	return Blocks.AIR;
}

BlockID ChunkContainer::GetBlockUnsafe(int x, int y, int z) const {
	return BlockStorage.GetBlock(x, y, z);
}

void ChunkContainer::SetBlock(BlockID block, int x, int y, int z) {
	if (!((x | y | z) >> 4)) { //check if it is in the chunk
		SetBlockUnsafe(block, x, y, z);
		return;
	}

	int dx = (((x >> 31) & 0b1) + 1) & -(!!(x >> 4));
	int dy = (((y >> 31) & 0b1) + 2) & -(!!(y >> 4));
	int dz = (((z >> 31) & 0b1) + 4) & -(!!(z >> 4));

	if (dx) {
		OutsideBlockToPlace[dx - 1].emplace_back(block, x + NeighborOffset[(dx - 1) & 1], y, z);
		return;
	}
	if (dy) {
		OutsideBlockToPlace[dy].emplace_back(block, x, y + NeighborOffset[dy & 0b1], z);
		return;
	}
	if (dz) {
		OutsideBlockToPlace[dz].emplace_back(block, x, y, z + NeighborOffset[dz & 0b1]);
		return;
	}
}

void ChunkContainer::SetBlockUnsafe(BlockID block, int x, int y, int z) {
	BlockStorage.ChangeBlock(block, (uint32_t)x, (uint32_t)y, (uint32_t)z);

	isEmpty = false;
}

void ChunkContainer::SetPosition(int x, int y, int z) {
	position_.set(x, y, z);
}

ChunkContainer* ChunkContainer::GetNeighbor(unsigned int side) const {
	return Neighbors[side];
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