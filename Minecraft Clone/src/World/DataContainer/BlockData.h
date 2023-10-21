#pragma once
#include "../Chunk/Block/Blocks.h"
#include <glm/vec3.hpp>

class BlockContainer {
public:
	
	inline BlockID GetBlock(unsigned int x, unsigned int y, unsigned int z) {
		return Data[GetDataIndex(x,y,z)];
	}

	inline void ChangeBlock(BlockID ID, unsigned int x, unsigned int y, unsigned int z) {
		Data[GetDataIndex(x,y,z)] = ID;
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

	inline static unsigned int GetDataIndex(unsigned int x, unsigned int y, unsigned int z) {
		return (x << 8) | (z << 4) | y;
	}
	
	BlockID Data[4096]{Blocks.AIR};
};
