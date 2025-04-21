#pragma once
#include "Level/Block/Block.h"
#include "Core/Typenames.h"

class Dimension;

struct GravityBlock : Block {
    GravityBlock();
    void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};