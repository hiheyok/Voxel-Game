#pragma once
#include "../Block.h"
#include "../../../Typenames.h"

class Dimension;

struct FluidProperties {
    int spread_rate_ = 1; // Ticks for fluid to spread
};

struct Fluid : Block {

    FluidProperties properties_;

    void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};