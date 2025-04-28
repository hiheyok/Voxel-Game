#pragma once
#include "Core/Typenames.h"
#include "Level/Block/Block.h"

class Dimension;

struct FluidProperties {
    int spread_rate_ = 1;  // Ticks for fluid to spread
};

struct Fluid : Block {
    Fluid(int spread_rate);
    FluidProperties fluid_properties_;
    // Use custom block model for fluids
    void InitializeBlockModel(ModelLoader& modelLoader) override;
    void Tick(const BlockPos& pos, Dimension* currentWorld) override;
};