#include "Level/Block/Block.h"
#include "RenderEngine/BlockModel/BlockModels.h"
Block::~Block() = default;
Block::Block() = default;
BlockType::BlockType(bool transparent, bool solid, bool fluid) {
    transparency_ = transparent;
    is_solid_ = solid;
    is_fluid_ = fluid;
}

BlockType::BlockType() = default;

