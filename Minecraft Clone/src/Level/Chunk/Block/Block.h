#pragma once
#include "Texture/BlockTexture.h"
#include "../../Typenames.h"
#include <vector>
#include "../../../RenderEngine/BlockModel/BlockModels.h"
#include "../../Chunk/ChunkPos/ChunkPos.h"

struct BlockType {

	BlockType(bool transparent, bool solid, bool fluid) {
		transparency = transparent;
		isSolid = solid;
		isFluid = fluid;
	}

	BlockType() {

	}

	bool isFluid = NULL;
	bool transparency = NULL;
	bool isSolid = NULL;
};

struct Block {

	virtual ~Block() {
		Clean();
	}

	void Clean() {
		if (Properties != nullptr) {
			delete Properties;
		}
		if (Texture != nullptr) {
			delete Texture;
		}
	}

	virtual void tick(const BlockPos& pos) {

	}

	static void* DimensionPTR;
	static void* serverPTR;

	BlockID ID = NULL;

	BlockType* Properties = nullptr;

	BlockTexture* Texture = nullptr;

	ModelV2::BlockModelV2* BlockModelData = NULL;
	

	std::string BlockName = "";
};

__declspec(selectany) void* Block::DimensionPTR = nullptr;
__declspec(selectany) void* Block::serverPTR = nullptr;