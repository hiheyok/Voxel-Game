#include "WorldInteraction.h"

void WorldInteraction::Interact(Player* player, const UserInputs& Inputs) {
	Ray ray;
	ray.Origin = player->properties_.position_;
	ray.Direction = dvec3(
		cos(player->properties_.rotation_.x * 0.0174533) * cos(player->properties_.rotation_.y * 0.0174533),
		sin(player->properties_.rotation_.y * 0.0174533),
		sin(player->properties_.rotation_.x * 0.0174533) * cos(player->properties_.rotation_.y * 0.0174533));

	Dimension* world = static_cast<Dimension*>(Block::dimension_ptr_);

	BlockID block = player->entity_inventory_.GetItem(player->entity_inventory_.right_hand_slot_).item_.GetBlock();

	if (Inputs.Mouse.RIGHT == Inputs.Mouse.PRESS) {
		PlaceBlock(ray, block, world);
	}

	if (Inputs.Mouse.LEFT == Inputs.Mouse.PRESS) {
		BreakBlock(ray, world);
	}
	if (Inputs.Mouse.MIDDLE == Inputs.Mouse.PRESS) {
		BlockID newBlock = GetBlock(ray, world);
		if (newBlock != Blocks.AIR) {
			player->entity_inventory_.SetSlot(player->entity_inventory_.right_hand_slot_, ItemStack{ g_items.GetItem(g_items.GetBlockItem(newBlock)) });
		}
		
	}
}

BlockID WorldInteraction::GetBlock(Ray ray, Dimension* dimension) {
	if (dimension->world_interactions_.collusions_.CheckRayIntersection(ray)) {
		return dimension->world_interactions_.GetBlock(BlockPos{ (int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z) });
	}
	return Blocks.AIR;
}

void WorldInteraction::BreakBlock(Ray ray, Dimension* dimension) {
	if (dimension->world_interactions_.collusions_.CheckRayIntersection(ray)) {
		Event::BlockEvent breakBlock;
		breakBlock.block_= Blocks.AIR;
		breakBlock.pos_ = BlockPos{ (int)floor(ray.EndPoint.x),
									(int)floor(ray.EndPoint.y),
									(int)floor(ray.EndPoint.z) };
		breakBlock.id_ = EventHandler.BlockPlace;
		dimension->event_manager_.AddEvent(breakBlock);

	}
}

void WorldInteraction::PlaceBlock(Ray ray, BlockID block, Dimension* dimension) {
	if (dimension->world_interactions_.collusions_.CheckRayIntersection(ray)) {
		int BounceSurface = ray.bouncesurface;

		ivec3 PlacePos = floor(ray.EndPoint);

		PlacePos[(int)floor(BounceSurface >> 1)] += (BounceSurface & 0b1) - ((BounceSurface + 1) & 0b1); //Offsets block location to be placed by 1 block

		Event::BlockEvent placeBlock;

		placeBlock.block_ = block;
		placeBlock.pos_ = BlockPos{ PlacePos.x, PlacePos.y, PlacePos.z };
		placeBlock.id_ = EventHandler.BlockPlace;

		dimension->event_manager_.AddEvent(placeBlock);

	}

}