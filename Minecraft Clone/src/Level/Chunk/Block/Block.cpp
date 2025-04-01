#include "Level/Chunk/Block/Block.h"
#include "RenderEngine/BlockModel/BlockModels.h"

BlockType::BlockType(bool transparent, bool solid, bool fluid) {
    transparency_ = transparent;
    is_solid_ = solid;
    is_fluid_ = fluid;
}

BlockType::BlockType() = default;

Block::Block() = default;
Block::~Block() = default;
