#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Texture/BlockTexture.h" // TODO: Remove this later
#include "../../Typenames.h"

class Dimension;
class BlockTexture;

namespace Model {
    struct BlockModel;
}

struct BlockType {

    BlockType(bool transparent, bool solid, bool fluid);

    BlockType();

    bool is_fluid_ = false;
    bool transparency_ = false;
    bool is_solid_ = false;
};

struct Block {
    BlockID id_ = NULL;
    std::unique_ptr<Model::BlockModel> block_model_data_ = nullptr;
    std::unique_ptr<BlockTexture> texture_ = nullptr;
    std::unique_ptr<BlockType> properties_ = nullptr;

    std::string block_name_ = "";

    Block();
    virtual ~Block();

    virtual void Tick(const BlockPos& pos, Dimension* currentWorld) = 0;
};
