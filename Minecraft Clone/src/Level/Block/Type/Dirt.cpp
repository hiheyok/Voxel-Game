#include "Level/Block/Type/Dirt.h"

#include "Level/Dimension/Dimension.h"

DirtBlock::DirtBlock() {
    properties_->is_solid_ = true;
    properties_->transparency_ = false;
    properties_->is_fluid_ = false;
    properties_->light_pass_ = false;
}

void DirtBlock::Tick(const BlockPos& pos, Dimension* currentWorld) {
    (void)currentWorld;
    (void)pos;
}