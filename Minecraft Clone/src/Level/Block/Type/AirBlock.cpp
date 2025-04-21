#include "Level/Block/Type/AirBlock.h"

AirBlock::AirBlock() {
    properties_->is_solid_ = false;
    properties_->transparency_ = true;
    properties_->is_fluid_ = false;
    properties_->light_pass_ = true;
}

void AirBlock::Tick(const BlockPos& pos, Dimension* currentWorld) {

}