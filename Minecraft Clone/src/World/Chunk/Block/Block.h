#pragma once
#include "Texture/BlockTexture.h"

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

struct Block {

	~Block() {
		delete Properties;
		delete Texture;
	}

	virtual void tick(int x, int y, int z) {
	}

	static void* WorldPTR;

	BlockID ID = NULL;

	BlockType* Properties = nullptr;

	BlockTexture* Texture = nullptr;
};

__declspec(selectany) void* Block::WorldPTR = nullptr;