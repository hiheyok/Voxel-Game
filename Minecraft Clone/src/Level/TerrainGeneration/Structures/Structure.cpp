#include "Level/TerrainGeneration/Structures/Structure.h"

#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"

SetBlockRelative::SetBlockRelative(BlockID block, BlockPos pos)
    : pos_{pos}, block_{block} {}
SetBlockRelative::~SetBlockRelative() = default;
