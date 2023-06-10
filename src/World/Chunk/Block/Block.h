#pragma once

typedef unsigned short BlockID;

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