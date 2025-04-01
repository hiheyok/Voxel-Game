#include "Level/Chunk/Block/Material/BlockMaterial.h"
#include "Level/Chunk/Block/Type/BlockTypes.h"

Material::~Material() = default;

MaterialNone::MaterialNone() {
    type_ = MATERIAL_NONE;
}

Block* MaterialNone::BuildNewBlockType() {
    return static_cast<Block*>(new DefaultBlock());
}

MaterialFluid::MaterialFluid(int spreadRate) { //Spreadrate in ticks
    type_ = MATERIAL_FLUID;

    spread_ = spreadRate;
}

Block* MaterialFluid::BuildNewBlockType() {
    Fluid* fluid = new Fluid;
    fluid->properties_.spread_rate_ = spread_;

    return static_cast<Block*>(fluid);
}

MaterialGrass::MaterialGrass(double spreadChance, double breakChance) {
    type_ = MATERIAL_GRASS;
    spread_chance_ = spreadChance;
    break_chance_ = breakChance;
}

Block* MaterialGrass::BuildNewBlockType() {
    GrassBlock* grass = new GrassBlock;
    grass->properties_.break_chance_ = break_chance_;
    grass->properties_.spread_chance_ = spread_chance_;

    return static_cast<Block*>(grass);
}

MaterialDirt::MaterialDirt() {
    type_ = MATERIAL_DIRT;
}

Block* MaterialDirt::BuildNewBlockType() {
    DirtBlock* dirt = new DirtBlock();

    return static_cast<Block*>(dirt);
}

MaterialGravity::MaterialGravity() {
    type_ = MATERIAL_GRAVITY;
}

Block* MaterialGravity::BuildNewBlockType() {
    GravityBlock* gravity = new GravityBlock();

    return static_cast<Block*>(gravity);
}