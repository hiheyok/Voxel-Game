#pragma once
#include <vector>
#include "Texture/BlockTexture.h"
#include "../../Typenames.h"
#include "../../../RenderEngine/BlockModel/BlockModels.h"
#include "../../Chunk/ChunkPos/ChunkPos.h"

class Dimension;

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
        // Clean();
    }

    virtual void Tick(const BlockPos& pos, Dimension* currentWorld) = 0;

    BlockID id_ = NULL;
    std::unique_ptr<BlockType> properties_ = nullptr;
    std::unique_ptr<BlockTexture> texture_ = nullptr;

    std::unique_ptr<ModelV2::BlockModelV2> block_model_data_ = nullptr;
    std::string block_name_ = "";
};
