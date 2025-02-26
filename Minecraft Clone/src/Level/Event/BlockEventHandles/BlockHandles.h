#pragma once
#include "../../Chunk/Block/Block.h"

void HandlePlaceBlock(BlockID block, const BlockPos& pos);
void HandleDirtTick(BlockID block, const BlockPos& pos);
void HandleBlockTick(BlockID block, const BlockPos& pos);