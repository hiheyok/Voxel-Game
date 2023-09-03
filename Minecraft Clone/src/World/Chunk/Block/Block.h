#pragma once

typedef unsigned char BlockID;

struct BlockType {

	BlockType(bool transparent, bool solid, bool fluid) {
		transparency = transparent;
		isSolid = solid;
		isFluid = fluid;
	}

	bool isFluid = NULL;
	bool transparency = NULL;
	bool isSolid = NULL;
};