#include "Level/Block/Type/DefaultBlock.h"
#include "Level/Dimension/Dimension.h"

DefaultBlock::DefaultBlock() {
    properties_->is_solid_ = true;
    properties_->transparency_ = false;
    properties_->is_fluid_ = false;
    properties_->light_pass_ = false;
}

void DefaultBlock::Tick(const BlockPos& pos, Dimension* currentWorld) {
    (void)currentWorld;
    (void)pos;
}