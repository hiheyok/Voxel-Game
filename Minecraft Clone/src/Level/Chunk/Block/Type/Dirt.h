#pragma once
#include "../Block.h"
#include "../../../Typenames.h"

class Dimension;

struct DirtBlock : Block {

    void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};