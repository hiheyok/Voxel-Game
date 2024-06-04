#pragma once
#include "../../Chunk/Block/Block.h"

void HandlePlaceBlock(BlockID block, int x, int y, int z);
void HandleDirtTick(BlockID block, int x, int y, int z);
void HandleBlockTick(BlockID block, int x, int y, int z);