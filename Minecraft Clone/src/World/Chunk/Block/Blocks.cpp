#include "Blocks.h"
#include "Type/BlockTypes.h"

BlockID BlockList::QueueRegister(Material* material, bool transparency, bool solid, bool isFluid) {
	BlockRegistration reg;
	reg.blockID = BlockTypeCount;
	reg.isFluid = isFluid;
	reg.solid = solid;
	reg.transparency = transparency;
	reg.material = material;

	BlockTypeCount++;

	RegistrationQueue.push_back(reg);

	return BlockTypeCount - 1;
}

void BlockList::RegisterAll() {
	while (!RegistrationQueue.empty()) {
		BlockRegistration reg = RegistrationQueue.front();
		RegistrationQueue.pop_front();

		RegisterNewBlock(reg);

	}
}

void BlockList::RegisterNewBlock(BlockRegistration reg) {
	bool transparency = reg.transparency;
	bool solid = reg.solid;
	bool isFluid = reg.isFluid;

	BlockID ID = reg.blockID;
	BlockType* NewBlock = new BlockType(transparency, solid, isFluid);

	Block* block = reg.material->BuildNewBlockType();

	MaterialType Type = reg.material->type;

	

	block->ID = ID;
	block->Properties = NewBlock;
	block->Texture = new BlockTexture;

	BlockTypeData[ID] = block;

	
	Logger.LogInfo("Register", "Registered new block (ID): " + std::to_string(ID));

	delete reg.material;
	
}