#include "Chunk.h"

void Chunk::UpdateGen() {
	for (int side = 0; side < 6; side++) {
		int axis = side >> 1;
		int face = side & 0b1;

		if (Neighbors[side] == nullptr)
			continue;

		if (Neighbors[side]->OutsideBlockToPlace[axis * 2 + (!face)].size() == 0) {
			continue;
		}

		std::vector<SetBlockRelative> blocks = Neighbors[side]->OutsideBlockToPlace[axis * 2 + (!face)];

		Neighbors[side]->OutsideBlockToPlace[axis * 2 + (!face)].clear();

		for (int i = 0; i < blocks.size(); i++) {
			SetBlock(blocks[i].m_block, blocks[i].m_x, blocks[i].m_y, blocks[i].m_z);
		}

		blocks.clear();
	}
}