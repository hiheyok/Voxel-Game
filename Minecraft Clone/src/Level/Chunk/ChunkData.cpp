#include "ChunkData.h"

static const int NeighborOffset[2] = {
	  -16, 16
};


BlockID ChunkContainer::GetBlock(int x, int y, int z) const {
	if (!((x | y | z) >> 4)) { //check if it is in the chunk
		return block_storage_.GetBlock(x, y, z);
	}

	int dx = (((x >> 31) & 0b1) + 1) & -(!!(x >> 4));
	int dy = (((y >> 31) & 0b1) + 2) & -(!!(y >> 4));
	int dz = (((z >> 31) & 0b1) + 4) & -(!!(z >> 4));

	//Set to 0 if neighbor is null
	dx &= -!!(neighbors_[dx - 1]);
	dy &= -!!(neighbors_[dy]);
	dz &= -!!(neighbors_[dz]);

	if (dx != 0) return neighbors_[dx - 1]->GetBlock(x + NeighborOffset[(dx - 1) & 1], y, z );
	if (dy != 0) return neighbors_[dy]->GetBlock(x, y + NeighborOffset[dy & 0b1], z );
	if (dz != 0) return neighbors_[dz]->GetBlock(x, y, z + NeighborOffset[dz & 0b1] );

	return g_blocks.AIR;
}

BlockID ChunkContainer::GetBlockUnsafe(int x, int y, int z) const {
	return block_storage_.GetBlock(x, y, z);
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
		outside_block_to_place_[dx - 1].emplace_back(block, x + NeighborOffset[(dx - 1) & 1], y, z);
		return;
	}
	if (dy) {
		outside_block_to_place_[dy].emplace_back(block, x, y + NeighborOffset[dy & 0b1], z);
		return;
	}
	if (dz) {
		outside_block_to_place_[dz].emplace_back(block, x, y, z + NeighborOffset[dz & 0b1]);
		return;
	}
}

void ChunkContainer::SetBlockUnsafe(BlockID block, int x, int y, int z) {
	block_storage_.ChangeBlock(block, (uint32_t)x, (uint32_t)y, (uint32_t)z);

	is_empty_ = false;
}

void ChunkContainer::SetPosition(int x, int y, int z) {
	position_.set(x, y, z);
}

ChunkContainer* ChunkContainer::GetNeighbor(unsigned int side) const {
	return neighbors_[side];
}

void ChunkContainer::SetNeighbor(ChunkContainer* Neighbor, unsigned int Side) {
	neighbors_[Side] = Neighbor;
}

void ChunkContainer::ClearNeighbors() {
	for (int i = 0; i < 6; i++) {
		neighbors_[i] = nullptr;
	}
}

void ChunkContainer::Use() {
	while (in_use_) {};
	in_use_ = true;
}

void ChunkContainer::Unuse() {
	in_use_ = false;
}