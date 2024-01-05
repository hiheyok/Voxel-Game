#include "Blocks.h"
#include "Type/BlockTypes.h"

BlockID BlockList::RegisterNewBlock(Material* material, bool transparency, bool solid, bool isFluid) {
	BlockID ID = (BlockID)BlockTypeData.size();
	BlockType* NewBlock = new BlockType(transparency, solid, isFluid);

	Block* block;

	MaterialType Type = material->type;

	switch (Type) {
	case MATERIAL_DIRT:
		block = new DirtBlock;
		break;
	case MATERIAL_GRASS:
		block = new GrassBlock;
		break;
	case MATERIAL_FLUID:
		block = new Fluid;
		break;
	case MATERIAL_NONE:
		block = new Block;
		break;
	}

	material->SetBlockProperties(block);

	block->ID = ID;
	block->Properties = NewBlock;
	block->Texture = new BlockTexture;

	BlockTypeData.emplace(ID, block);

	
	Logger.LogInfo("Register", "Registered new block (ID): " + std::to_string(ID));


	//delete material; idk why this break
	return ID;
}