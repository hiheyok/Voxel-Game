#pragma once
#include "Level/Block/Block.h"
#include "Core/Typenames.h"

class Dimension;

struct AirBlock : Block {
    AirBlock();
    void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};