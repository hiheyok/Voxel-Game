#include "Level/Block/Type/SolidBlock.h"

SolidBlock::SolidBlock() { properties_->light_pass_ = false; }

void SolidBlock::Tick(BlockPos pos, Dimension* currentWorld) {}
