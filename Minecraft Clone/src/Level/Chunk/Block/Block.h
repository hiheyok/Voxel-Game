#pragma once
#include "Texture/BlockTexture.h"
#include "../../Typenames.h"
#include <vector>
#include "../../../RenderEngine/BlockModel/BlockModels.h"
#include "../../Chunk/ChunkPos/ChunkPos.h"

struct BlockType {

	BlockType(bool transparent, bool solid, bool fluid) {
		transparency_ = transparent;
		is_solid_ = solid;
		is_fluid_ = fluid;
	}

	BlockType() {

	}

	bool is_fluid_ = NULL;
	bool transparency_ = NULL;
	bool is_solid_ = NULL;
};

struct Block {
	virtual ~Block() {
		Clean();
	}

	void Clean() {
		if (properties_ != nullptr) {
			delete properties_;
		}
		if (texture_ != nullptr) {
			delete texture_;
		}
	}

	virtual void Tick(const BlockPos& pos) {

	}

	static void* dimension_ptr_;
	static void* server_ptr_;

	BlockID id_ = NULL;
	BlockType* properties_ = nullptr;
	BlockTexture* texture_ = nullptr;

	ModelV2::BlockModelV2* block_model_data_ = NULL;
	std::string block_name_ = "";
};

__declspec(selectany) void* Block::dimension_ptr_ = nullptr;
__declspec(selectany) void* Block::server_ptr_ = nullptr;