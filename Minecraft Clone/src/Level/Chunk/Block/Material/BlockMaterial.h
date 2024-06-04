#pragma once
#include "../Block.h"

enum MaterialType {
	MATERIAL_DIRT, MATERIAL_GRASS, MATERIAL_FLUID, MATERIAL_NONE, MATERIAL_GRAVITY
};

struct Material {
	MaterialType type = MATERIAL_NONE;

	virtual ~Material() {

	}

	virtual Block* BuildNewBlockType() {
		return new Block();
	}
};

struct MaterialNone : Material {
	MaterialNone() {
		type = MATERIAL_NONE;
	}

	Block* BuildNewBlockType() override;
};

struct MaterialFluid : Material {
	int Spread = 1;

	MaterialFluid(int SpreadRate) { //Spreadrate in ticks
		type = MATERIAL_FLUID;

		Spread = SpreadRate;
	}

	Block* BuildNewBlockType() override;
};

struct MaterialDirt : Material {
	MaterialDirt() {
		type = MATERIAL_DIRT;
	}

	Block* BuildNewBlockType() override;
};

struct MaterialGrass : Material {
	double SpreadChance = 0.001;
	double BreakChance = 0.001;

	MaterialGrass(double _SpreadChance, double _BreakChance) {
		type = MATERIAL_GRASS;
		SpreadChance = _SpreadChance;
		BreakChance = _BreakChance;
	}

	Block* BuildNewBlockType() override;
};

struct MaterialGravity : Material {
	MaterialGravity() {
		type = MATERIAL_GRAVITY;
	}

	Block* BuildNewBlockType() override;
};