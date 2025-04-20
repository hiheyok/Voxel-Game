#pragma once
#include "../Block.h"
#include "Core/Typenames.h"

class Dimension;

struct GrassProperties {
    double spread_chance_ = 0.001;
    double break_chance_ = 0.001;
};

struct GrassBlock : Block {

    GrassBlock();

    GrassProperties grass_properties_;

    void Tick(const BlockPos& pos, Dimension* currentWorld) override;

    bool GrassDestroyTick(Dimension* currentWorld, const BlockPos& pos);

    bool GrassSpreadTick(Dimension* currentWorld, const BlockPos& pos);
};