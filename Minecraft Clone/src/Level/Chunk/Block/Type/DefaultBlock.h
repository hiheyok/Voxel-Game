#pragma once
#include "../../ChunkPos/ChunkPos.h"
#include "../Block.h"

class Dimension;

struct DefaultBlock : Block {

    void Tick(const BlockPos& pos, Dimension* currentWorld) override {

    }
};