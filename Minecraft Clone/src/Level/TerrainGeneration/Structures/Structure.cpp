#include "Level/TerrainGeneration/Structures/Structure.h"

SetBlockRelative::SetBlockRelative(BlockID block, int8_t x, int8_t y, int8_t z) : x_{ x }, y_{ y }, z_{ z }, block_{ block } {}

void SetBlockRelative::SetPos(int8_t x, int8_t y, int8_t z) {
    x_ = x;
    y_ = y;
    z_ = z;
}