#include "Level/TerrainGeneration/Structures/Structure.h"

SetBlockRelative::SetBlockRelative(BlockID block, BlockPos pos)
    : pos_{pos}, block_{block} {}
SetBlockRelative::~SetBlockRelative() = default;
