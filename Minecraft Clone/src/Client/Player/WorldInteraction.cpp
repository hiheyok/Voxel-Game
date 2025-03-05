#include "WorldInteraction.h"

void WorldInteraction::Interact(Player* player, const UserInputs& inputs) {
    Ray ray;
    ray.origin_ = player->properties_.position_;
    ray.direction_ = glm::dvec3(
        cos(player->properties_.rotation_.x * 0.0174533) * cos(player->properties_.rotation_.y * 0.0174533),
        sin(player->properties_.rotation_.y * 0.0174533),
        sin(player->properties_.rotation_.x * 0.0174533) * cos(player->properties_.rotation_.y * 0.0174533));

    Dimension* world = static_cast<Dimension*>(Block::dimension_ptr_);

    BlockID block = player->entity_inventory_.GetItem(player->entity_inventory_.right_hand_slot_).item_.GetBlock();

    if (inputs.mouse_.right_ == inputs.mouse_.PRESS) {
        PlaceBlock(ray, block, world);
    }

    if (inputs.mouse_.left_ == inputs.mouse_.PRESS) {
        BreakBlock(ray, world);
    }
    if (inputs.mouse_.middle_ == inputs.mouse_.PRESS) {
        BlockID newBlock = GetBlock(ray, world);
        if (newBlock != g_blocks.AIR) {
            player->entity_inventory_.SetSlot(player->entity_inventory_.right_hand_slot_, ItemStack{ g_items.GetItem(g_items.GetBlockItem(newBlock)) });
        }
        
    }
}

BlockID WorldInteraction::GetBlock(Ray ray, Dimension* dimension) {
    if (dimension->world_interactions_.collusions_.CheckRayIntersection(ray)) {
        return dimension->world_interactions_.GetBlock(BlockPos{ (int)floor(ray.end_point_.x), (int)floor(ray.end_point_.y), (int)floor(ray.end_point_.z) });
    }
    return g_blocks.AIR;
}

void WorldInteraction::BreakBlock(Ray ray, Dimension* dimension) {
    if (dimension->world_interactions_.collusions_.CheckRayIntersection(ray)) {
        Event::BlockEvent breakBlock;
        breakBlock.block_= g_blocks.AIR;
        breakBlock.pos_ = BlockPos{ (int)floor(ray.end_point_.x),
                                    (int)floor(ray.end_point_.y),
                                    (int)floor(ray.end_point_.z) };
        breakBlock.id_ = g_event_handler.BlockPlace;
        dimension->event_manager_.AddEvent(breakBlock);

    }
}

void WorldInteraction::PlaceBlock(Ray ray, BlockID block, Dimension* dimension) {
    if (dimension->world_interactions_.collusions_.CheckRayIntersection(ray)) {
        int BounceSurface = ray.bounce_surface_;

        glm::ivec3 placePos = floor(ray.end_point_);

        placePos[(int)floor(BounceSurface >> 1)] += (BounceSurface & 0b1) - ((BounceSurface + 1) & 0b1); //Offsets block location to be placed by 1 block

        Event::BlockEvent placeBlock;

        placeBlock.block_ = block;
        placeBlock.pos_ = BlockPos{ placePos.x, placePos.y, placePos.z };
        placeBlock.id_ = g_event_handler.BlockPlace;

        dimension->event_manager_.AddEvent(placeBlock);
    }
}