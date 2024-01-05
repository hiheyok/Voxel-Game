#include "BlockMaterial.h"
#include "../Type/BlockTypes.h"

void MaterialNone::SetBlockProperties(Block* block) {
	//Nothing
}

void MaterialFluid::SetBlockProperties(Block* block) {
	Fluid* fluid = static_cast<Fluid*>(block);

	fluid->Properties.SpreadRate = Spread;
}

void MaterialGrass::SetBlockProperties(Block* block) {
	GrassBlock* grass = static_cast<GrassBlock*>(block);
	grass->Properties.BreakChance = BreakChance;
	grass->Properties.SpreadChance = SpreadChance;
}

void MaterialDirt::SetBlockProperties(Block* block) {
	//Do nothing right now.
}