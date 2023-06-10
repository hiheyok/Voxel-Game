#pragma once
#include "../Chunk/Block/Blocks.h"

class BlockContainer {
public:
	
	BlockID GetBlock(unsigned int x, unsigned int y, unsigned int z) {
		return Data[GetDataIndex(x, y, z)];
	}

	void ChangeBlock(BlockID ID, unsigned int x, unsigned int y, unsigned int z) {
		Data[GetDataIndex(x, y, z)] = ID;
	}

	void DumpData() {
		std::ofstream file;
		file.open("Chunk.dmp");
		for (int i = 0; i < 4096; i++) {
			file << Data[i];
		}
		file.close();
	}


private:

	static unsigned int GetDataIndex(unsigned int x, unsigned int y, unsigned int z) {
		return (x * 256) + (z * 16) + y;
	}
	
	BlockID Data[4096]{AIR};
};