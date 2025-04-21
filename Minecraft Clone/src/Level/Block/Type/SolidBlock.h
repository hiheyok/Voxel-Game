#pragma once
#include "Level/Block/Block.h"
#include "Core/Typenames.h"

class Dimension;

struct SolidBlock : Block {
    SolidBlock();
    virtual void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};