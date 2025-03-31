#pragma once
#include "../Block.h"
#include "../../../Typenames.h"

class Dimension;

struct GravityBlock : Block {

    void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};