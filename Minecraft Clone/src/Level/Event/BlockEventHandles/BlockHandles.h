#pragma once
#include "../../Chunk/Block/Block.h"

class Dimension;

namespace Event {
    struct BlockEvent;
};

void HandlePlaceBlock(const Event::BlockEvent&, Dimension* dimension);
void HandleBlockTick(const Event::BlockEvent&, Dimension* dimension);