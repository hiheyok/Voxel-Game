#pragma once
#include "Core/Typenames.h"
#include "Level/Block/Block.h"

class Dimension;

struct DirtBlock : Block {
    DirtBlock();
    void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};