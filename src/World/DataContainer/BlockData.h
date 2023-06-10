#pragma once
#include "../Chunk/Block/Blocks.h"

class BlockContainer {
public:
	
	Block& GetBlock(unsigned int x, unsigned int y, unsigned int z) {
		return Data[GetDataIndex(x, y, x)];
	}

	void ChangeBlock(BlockID ID, unsigned int x, unsigned int y, unsigned int z) {
		Data[GetDataIndex(x, y, x)].ID = ID;
	}


private:

	static unsigned int GetDataIndex(unsigned int x, unsigned int y, unsigned int z) {
		return (x * 256) + (z * 16) + y;
	}
	
	Block Data[16 * 16 * 16]{};
};