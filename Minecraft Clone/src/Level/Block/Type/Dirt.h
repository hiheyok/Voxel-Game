#pragma once
#include "../Block.h"
#include "Core/Typenames.h"

class Dimension;

struct DirtBlock : Block {

    void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};