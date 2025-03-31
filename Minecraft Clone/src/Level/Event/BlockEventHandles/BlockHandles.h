#pragma once

class Dimension;

struct BlockEvent;

void HandlePlaceBlock(const BlockEvent&, Dimension* dimension);
void HandleBlockTick(const BlockEvent&, Dimension* dimension);