#include "BlockMaterial.h"
#include "../Type/BlockTypes.h"

Block* MaterialNone::BuildNewBlockType() {
	return new Block();
}

Block* MaterialFluid::BuildNewBlockType() {
	Fluid* fluid = new Fluid;
	fluid->Properties.SpreadRate = Spread;

	return static_cast<Block*>(fluid);
}

Block* MaterialGrass::BuildNewBlockType() {
	GrassBlock* grass = new GrassBlock;
	grass->Properties.BreakChance = BreakChance;
	grass->Properties.SpreadChance = SpreadChance;

	return static_cast<Block*>(grass);
}

Block* MaterialDirt::BuildNewBlockType() {
	DirtBlock* dirt = new DirtBlock();

	return static_cast<Block*>(dirt);
}