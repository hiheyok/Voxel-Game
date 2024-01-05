#pragma once
#include "../Block.h"

enum MaterialType {
	MATERIAL_DIRT, MATERIAL_GRASS, MATERIAL_FLUID, MATERIAL_NONE
};

struct Material {
	MaterialType type = MATERIAL_NONE;

	virtual void SetBlockProperties(Block* block) {

	}
};

struct MaterialNone : Material {
	MaterialNone() {
		type = MATERIAL_NONE;
	}

	void SetBlockProperties(Block* block) override;
};

struct MaterialFluid : Material {
	int Spread = 1;

	MaterialFluid(int SpreadRate) { //Spreadrate in ticks
		type = MATERIAL_FLUID;

		Spread = SpreadRate;
	}

	void SetBlockProperties(Block* block) override;
};

struct MaterialDirt : Material {
	MaterialDirt() {
		type = MATERIAL_DIRT;
	}

	void SetBlockProperties(Block* block) override;
};

struct MaterialGrass : Material {
	double SpreadChance = 0.001;
	double BreakChance = 0.001;

	MaterialGrass(double _SpreadChance, double _BreakChance) {
		type = MATERIAL_DIRT;
		SpreadChance = _SpreadChance;
		BreakChance = _BreakChance;
	}

	void SetBlockProperties(Block* block) override;
};