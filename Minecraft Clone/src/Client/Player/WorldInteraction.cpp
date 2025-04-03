#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "Client/Player/WorldInteraction.h"
#include "Client/ClientLevel/ClientCache.h"
#include "Client/IO/IO.h"
#include "Client/IO/KEY_CODE.h"
#include "Level/Entity/Mobs/Player.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Networking/PlayerAction.h"
#include "Utils/Math/vectorOperations.h"
#include "Utils/Math/Ray/Ray.h"

void WorldInteraction::Interact(Player* player, const UserInputs& inputs, ServerInterface* interface, ClientCache* cache) {
    Ray ray;
    ray.origin_ = player->properties_.position_;
    ray.direction_ = glm::dvec3(
        cos(player->properties_.rotation_.x * 0.0174533) * cos(player->properties_.rotation_.y * 0.0174533),
        sin(player->properties_.rotation_.y * 0.0174533),
        sin(player->properties_.rotation_.x * 0.0174533) * cos(player->properties_.rotation_.y * 0.0174533));

    BlockID block = player->entity_inventory_.GetItem(player->entity_inventory_.right_hand_slot_).item_.GetBlock();

    if (inputs.mouse_.right_ == inputs.mouse_.PRESS) {
        PlaceBlock(ray, block, interface, cache);
    }

    if (inputs.mouse_.left_ == inputs.mouse_.PRESS) {
        BreakBlock(ray, interface, cache);
    }
    if (inputs.mouse_.middle_ == inputs.mouse_.PRESS) {
        BlockID newBlock = GetBlock(ray, interface, cache);
        if (newBlock != g_blocks.AIR) {
            player->entity_inventory_.SetSlot(player->entity_inventory_.right_hand_slot_, ItemStack{ g_items.GetItem(g_items.GetBlockItem(newBlock)) });
        }
        
    }
}

// This will just get the block from the cache

BlockID WorldInteraction::GetBlock(Ray ray, ServerInterface* interface, ClientCache* cache) {
    if (cache->collusion_manager_.CheckRayIntersection(ray)) {
        BlockPos pos{ (int)floor(ray.end_point_.x), (int)floor(ray.end_point_.y), (int)floor(ray.end_point_.z) };
        return cache->GetBlock(pos);
    }
    return g_blocks.AIR;
}

void WorldInteraction::BreakBlock(Ray ray, ServerInterface* interface, ClientCache* cache) {
    if (cache->collusion_manager_.CheckRayIntersection(ray)) {
        // Send block break packet
        PlayerPacket::PlayerDestroyBlock packet;
        packet.pos_ = BlockPos{ (int)floor(ray.end_point_.x),
                                    (int)floor(ray.end_point_.y),
                                    (int)floor(ray.end_point_.z) };

        interface->SendPlayerAction(packet);
    }
}

void WorldInteraction::PlaceBlock(Ray ray, BlockID block, ServerInterface* interface, ClientCache* cache) {
    if (cache->collusion_manager_.CheckRayIntersection(ray)) {
        int BounceSurface = ray.bounce_surface_;

        glm::ivec3 placePos = floor(ray.end_point_);

        g_logger.LogDebug("WorldInteraction::PlaceBlock", std::to_string(placePos.y));

        placePos[(int)floor(BounceSurface >> 1)] += (BounceSurface & 0b1) - ((BounceSurface + 1) & 0b1); //Offsets block location to be placed by 1 block
        PlayerPacket::PlayerPlaceBlock packet;
        packet.block_ = block;
        packet.pos_ = BlockPos{ placePos.x, placePos.y, placePos.z };
        interface->SendPlayerAction(packet);
    }
}