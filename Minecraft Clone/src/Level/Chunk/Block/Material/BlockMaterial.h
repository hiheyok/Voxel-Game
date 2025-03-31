#pragma once

struct Block;

enum MaterialType {
    MATERIAL_DIRT, MATERIAL_GRASS, MATERIAL_FLUID, MATERIAL_NONE, MATERIAL_GRAVITY
};

struct Material {
    MaterialType type_ = MATERIAL_NONE;

    virtual ~Material();

    virtual Block* BuildNewBlockType() = 0;
};

struct MaterialNone : Material {
    MaterialNone();

    Block* BuildNewBlockType() override;
};

struct MaterialFluid : Material {
    int spread_ = 1;

    MaterialFluid(int spreadRate);

    Block* BuildNewBlockType() override;
};

struct MaterialDirt : Material {
    MaterialDirt();

    Block* BuildNewBlockType() override;
};

struct MaterialGrass : Material {
    double spread_chance_ = 0.001;
    double break_chance_ = 0.001;

    MaterialGrass(double spreadChance, double breakChance);

    Block* BuildNewBlockType() override;
};

struct MaterialGravity : Material {
    MaterialGravity();

    Block* BuildNewBlockType() override;
};