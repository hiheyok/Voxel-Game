#pragma once
#include "Core/Typenames.h"
#include "Level/Block/Block.h"

class Dimension;

struct DefaultBlock : Block {
  DefaultBlock();
  void Tick(BlockPos pos, Dimension* currentWorld) override;
};
