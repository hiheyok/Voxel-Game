#include "BlockMaterial.h"
#include "../Type/BlockTypes.h"

Block* MaterialNone::BuildNewBlockType() {
    return static_cast<Block*>(new DefaultBlock());
}

Block* MaterialFluid::BuildNewBlockType() {
    Fluid* fluid = new Fluid;
    fluid->properties_.spread_rate_ = spread_;

    return static_cast<Block*>(fluid);
}

Block* MaterialGrass::BuildNewBlockType() {
    GrassBlock* grass = new GrassBlock;
    grass->properties_.break_chance_ = break_chance_;
    grass->properties_.spread_chance_ = spread_chance_;

    return static_cast<Block*>(grass);
}

Block* MaterialDirt::BuildNewBlockType() {
    DirtBlock* dirt = new DirtBlock();

    return static_cast<Block*>(dirt);
}

Block* MaterialGravity::BuildNewBlockType() {
    GravityBlock* gravity = new GravityBlock();

    return static_cast<Block*>(gravity);
}