#pragma once
#include "../Chunk/Block/Blocks.h"
#include <cctype>

class BlockContainer {
public:
	
	inline const BlockID& GetBlock(unsigned int x, unsigned int y, unsigned int z) const {
		return data_[GetDataIndex(x,y,z)];
	}

	inline const void ChangeBlock(BlockID ID, unsigned int x, unsigned int y, unsigned int z) {
		data_[GetDataIndex(x,y,z)] = ID;
	}

	inline const BlockID& GetBlockUnsafe(unsigned int index) const {
		return data_[index];
	}

	inline void ChangeBlockUnsafe(BlockID ID, unsigned int index) {
		data_[index] = ID;
	}

	inline const BlockID* GetContainerData() const {
		return data_;
	}

private:

	inline constexpr const static uint16_t GetDataIndex(unsigned int x, unsigned int y, unsigned int z) {
		return (x << 8) | (z << 4) | y;
	}
	
	BlockID data_[4096]{Blocks.AIR};
};
