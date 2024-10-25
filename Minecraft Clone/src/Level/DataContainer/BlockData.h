#pragma once
#include "../Chunk/Block/Blocks.h"

class BlockContainer {
public:
	
	inline const BlockID& GetBlock(unsigned int x, unsigned int y, unsigned int z) {
		return Data[GetDataIndex(x,y,z)];
	}

	inline const void ChangeBlock(BlockID ID, unsigned int x, unsigned int y, unsigned int z) {
		Data[GetDataIndex(x,y,z)] = ID;
	}

	inline const BlockID& GetBlockUnsafe(unsigned int index) {
		return Data[index];
	}

	inline void ChangeBlockUnsafe(BlockID ID, unsigned int index) {
		Data[index] = ID;
	}

	inline const BlockID* getContainerData() {
		return Data;
	}

private:

	constexpr inline const static uint16_t GetDataIndex(unsigned int x, unsigned int y, unsigned int z) {
		return (x << 8) | (z << 4) | y;
	}
	
	BlockID Data[4096]{Blocks.AIR};
};
