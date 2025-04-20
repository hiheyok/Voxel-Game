#pragma once
#include "../Block.h"
#include "Core/Typenames.h"

class Dimension;

struct DirtBlock : Block {
    DirtBlock();
    void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};