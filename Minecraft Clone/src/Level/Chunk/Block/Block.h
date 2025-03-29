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
        // Clean();
    }

    virtual void Tick(const BlockPos& pos) {
        (void)pos;
    }

    static void* dimension_ptr_;
    static void* server_ptr_;

    BlockID id_ = NULL;
    std::unique_ptr<BlockType> properties_ = nullptr;
    std::unique_ptr<BlockTexture> texture_ = nullptr;

    std::unique_ptr<ModelV2::BlockModelV2> block_model_data_ = nullptr;
    std::string block_name_ = "";
};

inline void* Block::dimension_ptr_ = nullptr;
inline void* Block::server_ptr_ = nullptr;