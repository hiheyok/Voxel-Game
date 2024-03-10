#include "WorldInteraction.h"

void WorldInteraction::Interact(Player* player, UserInputs Inputs) {
	Ray ray;
	ray.Origin = player->Properties.Position;
	ray.Direction = dvec3(
		cos(player->Properties.Rotation.x * 0.0174533) * cos(player->Properties.Rotation.y * 0.0174533),
		sin(player->Properties.Rotation.y * 0.0174533),
		sin(player->Properties.Rotation.x * 0.0174533) * cos(player->Properties.Rotation.y * 0.0174533));

	World* world = static_cast<World*>(Block::WorldPTR);

	BlockID block = player->m_EntityInventory.GetItem(player->m_EntityInventory.RightHandSlot).m_item.GetBlock();

	if (Inputs.Mouse.RIGHT == Inputs.Mouse.PRESS) {
		PlaceBlock(ray, block, world);
	}

	if (Inputs.Mouse.LEFT == Inputs.Mouse.PRESS) {
		BreakBlock(ray, world);
	}
	if (Inputs.Mouse.MIDDLE == Inputs.Mouse.PRESS) {
		BlockID newBlock = GetBlock(ray, world);
		player->m_EntityInventory.SetSlot(player->m_EntityInventory.RightHandSlot, ItemStack(Items.GetItem(Items.GetBlockItem(newBlock))));
	}
}

BlockID WorldInteraction::GetBlock(Ray ray, World* world) {
	if (world->RayIntersection(ray)) {
		return world->GetBlock((int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z));
	}
}

void WorldInteraction::BreakBlock(Ray ray, World* world) {
	if (world->RayIntersection(ray)) {
		Event placeBlock;
		placeBlock.Type = BLOCK_EVENT;
		placeBlock.Data.BlockEvent.block = Blocks.AIR;
		placeBlock.Data.BlockEvent.x = (int)floor(ray.EndPoint.x);
		placeBlock.Data.BlockEvent.y = (int)floor(ray.EndPoint.y);
		placeBlock.Data.BlockEvent.z = (int)floor(ray.EndPoint.z);
		placeBlock.Data.BlockEvent.id = EventHandler.BlockPlace;
		world->QueueEvent(placeBlock);
	}
}

void WorldInteraction::PlaceBlock(Ray ray, BlockID block, World* world) {

	if (world->RayIntersection(ray)) {

		int BounceSurface = ray.bouncesurface;

		ivec3 PlacePos = floor(ray.EndPoint);

		PlacePos[(int)floor(BounceSurface >> 1)] += (BounceSurface & 0b1) - ((BounceSurface + 1) & 0b1); //Offsets block location to be placed by 1 block

		Event placeBlock;
		placeBlock.Type = BLOCK_EVENT;
		placeBlock.Data.BlockEvent.block = block;
		placeBlock.Data.BlockEvent.x = PlacePos.x;
		placeBlock.Data.BlockEvent.y = PlacePos.y;
		placeBlock.Data.BlockEvent.z = PlacePos.z;
		placeBlock.Data.BlockEvent.id = EventHandler.BlockPlace;

		world->QueueEvent(placeBlock);
	}

}