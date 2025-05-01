#pragma once
#include "Core/Typenames.h"
#include "Level/Block/Block.h"

class Dimension;

struct GravityBlock : Block {
  GravityBlock();
  void Tick(BlockPos pos, Dimension* currentWorld) override;
};
