#pragma once
#include "../../Chunk/Block/Block.h"

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

	void SetPos(char x, char y, char z);
};

struct Structure {
	std::vector<SetBlockRelative> Build;
};