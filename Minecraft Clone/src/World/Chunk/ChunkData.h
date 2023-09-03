#pragma once
#include "../DataContainer/BlockData.h"
#include <glm/vec3.hpp>
#include "ChunkID.h"
#include <atomic>

#define PX 0x00
#define NX 0x01
#define PY 0x02
#define NY 0x03
#define PZ 0x04
#define NZ 0x05

typedef unsigned long long int ChunkID;

namespace ChunkBlockPlace {
	struct SetBlockRelative {
		char m_x;
		char m_y;
		char m_z;

		BlockID m_block;

		SetBlockRelative() {

		}

		SetBlockRelative(BlockID block, char x, char y, char z) {
			m_x = x;
			m_y = y;
			m_z = z;
			m_block = block;
		}

		void SetPos(char x, char y, char z) {
			m_x = x;
			m_y = y;
			m_z = z;
		}
	};
}

class ChunkContainer {
	
public:
	
	void SetNeighbor(ChunkContainer* Neighbor, unsigned int Side);

	void ClearNeighbors();

	BlockID GetBlock(int x, int y, int z);

	void SetBlock(BlockID block, int x, int y, int z);

	ChunkContainer* GetNeighbor(unsigned int Side);
	
	void SetPosition(int x, int y, int z);

	glm::ivec3 Position;
	ChunkID chunkID;

	bool isEmpty = true;

	ChunkContainer* Neighbors[6]{nullptr};
	std::vector<ChunkBlockPlace::SetBlockRelative> OutsideBlockToPlace[6]{};
private:

	BlockContainer Blocks;
};