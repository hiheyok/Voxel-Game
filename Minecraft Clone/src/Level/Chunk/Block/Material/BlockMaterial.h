#pragma once
#include "../Block.h"

enum MaterialType {
	MATERIAL_DIRT, MATERIAL_GRASS, MATERIAL_FLUID, MATERIAL_NONE, MATERIAL_GRAVITY
};

struct Material {
	MaterialType type_ = MATERIAL_NONE;

	virtual ~Material() {

	}

	virtual Block* BuildNewBlockType() {
		return new Block();
	}
};

struct MaterialNone : Material {
	MaterialNone() {
		type_ = MATERIAL_NONE;
	}

	Block* BuildNewBlockType() override;
};

struct MaterialFluid : Material {
	int spread_ = 1;

	MaterialFluid(int spreadRate) { //Spreadrate in ticks
		type_ = MATERIAL_FLUID;

		spread_ = spreadRate;
	}

	Block* BuildNewBlockType() override;
};

struct MaterialDirt : Material {
	MaterialDirt() {
		type_ = MATERIAL_DIRT;
	}

	Block* BuildNewBlockType() override;
};

struct MaterialGrass : Material {
	double spread_chance_ = 0.001;
	double break_chance_ = 0.001;

	MaterialGrass(double spreadChance, double breakChance) {
		type_ = MATERIAL_GRASS;
		spread_chance_ = spreadChance;
		break_chance_ = breakChance;
	}

	Block* BuildNewBlockType() override;
};

struct MaterialGravity : Material {
	MaterialGravity() {
		type_ = MATERIAL_GRAVITY;
	}

	Block* BuildNewBlockType() override;
};