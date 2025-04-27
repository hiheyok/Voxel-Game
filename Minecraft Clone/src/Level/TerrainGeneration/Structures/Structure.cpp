#include "Level/TerrainGeneration/Structures/Structure.h"

SetBlockRelative::SetBlockRelative(BlockID block, const BlockPos& pos) : pos_{ pos }, block_{ block } {}
SetBlockRelative::~SetBlockRelative() = default;