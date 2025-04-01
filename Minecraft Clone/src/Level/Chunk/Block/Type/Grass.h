#pragma once
#include "../Block.h"
#include "Core/Typenames.h"

class Dimension;

struct GrassProperties {
    double spread_chance_ = 0.001;
    double break_chance_ = 0.001;
};

struct GrassBlock : Block {

    GrassProperties properties_;

    void Tick(const BlockPos& pos, Dimension* currentWorld) override;

    bool GrassDestroyTick(Dimension* currentWorld, const BlockPos& pos);

    //return true if there is no exposed dirt blocks surrounding it
    // TODO: refactor code
    bool GrassSpreadTick(Dimension* currentWorld, const BlockPos& pos);
};