#include "ChunkData.h"

BlockID ChunkContainer::GetBlock(int x, int y, int z) {
	if (x >= 16 || y >= 16 || z >= 16 || x < 0 || y < 0 || z < 0) {
		if (x < 0) {

			if (Neighbors[NX] != nullptr) {
				return Neighbors[NX]->GetBlock(x + 16, y, z);
			}
			else {
				return AIR;
			}

		}

		if (x >= 16) {
			if (Neighbors[PX] != nullptr) {
				return Neighbors[PX]->GetBlock(x - 16, y, z);

			}
			else {
				return AIR;
			}

		}

		if (y < 0) {

			if (Neighbors[NY] != nullptr) {
				return Neighbors[NY]->GetBlock(x, y + 16, z);
			}
			else {
				return AIR;
			}

		}

		if (y >= 16) {
			if (Neighbors[PY] != nullptr) {
				return Neighbors[PY]->GetBlock(x, y - 16, z);

			}
			else {
				return AIR;
			}

		}

		if (z < 0) {

			if (Neighbors[NZ] != nullptr) {
				return Neighbors[NZ]->GetBlock(x, y, z + 16);
			}
			else {
				return AIR;
			}

		}

		if (z >= 16) {
			if (Neighbors[PZ] != nullptr) {
				return Neighbors[PZ]->GetBlock(x, y, z - 16);

			}
			else {
				return AIR;
			}

		}


	}
	else {
		return Blocks.GetBlock(x, y, z);
	}
	return AIR;
}