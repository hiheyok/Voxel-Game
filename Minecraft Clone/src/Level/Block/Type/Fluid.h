#pragma once
#include "../Block.h"
#include "Core/Typenames.h"

class Dimension;

struct FluidProperties {
    int spread_rate_ = 1; // Ticks for fluid to spread
};

struct Fluid : Block {
    Fluid();
    FluidProperties fluid_properties_;
    // Use custom block model for fluids
    void InitializeBlockModel(ModelLoader& modelLoader) override;
    void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};