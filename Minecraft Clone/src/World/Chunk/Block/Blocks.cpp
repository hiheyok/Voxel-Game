#include "Blocks.h"
#include "Type/Grass.h"
#include "Type/Dirt.h"
#include "Type/Fluid.h"

BlockID BlockList::RegisterNewBlock(BlockMaterial material, bool transparency, bool solid, bool isFluid) {
	BlockID ID = (BlockID)BlockTypeData.size();
	BlockType* NewBlock = new BlockType(transparency, solid, isFluid);

	Block* block;

	switch (material) {
	case MATERIAL_DIRT:
		block = new DIRT_BLOCK;
		break;
	case MATERIAL_GRASS:
		block = new GRASS_BLOCK;
		break;
	case MATERIAL_FLUID:
		block = new FLUID_BLOCK;
		break;
	case MATERIAL_NONE:
		block = new Block;
		break;
	}

	block->ID = ID;
	block->Properties = NewBlock;
	block->Texture = new BlockTexture;

	BlockTypeData.emplace(ID, block);
	Logger.LogInfo("Register", "Registered new block (ID): " + std::to_string(ID));
	return ID;
}