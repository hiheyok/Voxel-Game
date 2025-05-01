#pragma once
#include "Core/Typenames.h"
#include "Level/Block/Block.h"

class Dimension;

struct SolidBlock : Block {
  SolidBlock();
  virtual void Tick(BlockPos pos, Dimension* currentWorld) override;
};
