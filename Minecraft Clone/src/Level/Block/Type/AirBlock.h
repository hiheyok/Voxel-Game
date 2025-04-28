#pragma once
#include "Core/Typenames.h"
#include "Level/Block/Block.h"

class Dimension;

struct AirBlock : Block {
    AirBlock();
    void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};