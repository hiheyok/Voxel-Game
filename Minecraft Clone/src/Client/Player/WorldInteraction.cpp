// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Player/WorldInteraction.h"

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "Client/ClientLevel/ClientCache.h"
#include "Client/Inputs/InputManager.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Networking/PlayerAction.h"
#include "Level/Block/Blocks.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Item/Items.h"
#include "Utils/Math/Ray/Ray.h"
#include "Utils/Math/vectorOperations.h"

WorldInteraction::WorldInteraction(GameContext& context) : context_{context} {}
WorldInteraction::~WorldInteraction() = default;

void WorldInteraction::Interact(Player* player, const InputManager& inputs,
                                ServerInterface* interface,
                                ClientCache* cache) {
  Ray ray;
  ray.origin_ = player->properties_.position_;
  ray.direction_ =
      glm::dvec3(cos(player->properties_.rotation_.x * kDegToRad) *
                     cos(player->properties_.rotation_.y * kDegToRad),
                 sin(player->properties_.rotation_.y * kDegToRad),
                 sin(player->properties_.rotation_.x * kDegToRad) *
                     cos(player->properties_.rotation_.y * kDegToRad));

  BlockID block = player->entity_inventory_
                      .GetItem(player->entity_inventory_.right_hand_slot_)
                      .item_.GetBlock();

  if (inputs.CheckAction(InputAction::kUseItemPlaceBlock)) {
    PlaceBlock(ray, block, interface, cache);
  }

  if (inputs.CheckAction(InputAction::kAttackDestroy)) {
    BreakBlock(ray, interface, cache);
  }
  if (inputs.CheckAction(InputAction::kPickBlock)) {
    BlockID newBlock = GetBlock(ray, cache);
    if (newBlock != context_.blocks_->AIR) {
      player->entity_inventory_.SetSlot(
          player->entity_inventory_.right_hand_slot_,
          ItemStack{context_.items_->GetItem(
              context_.items_->GetBlockItem(newBlock))});
    }
  }
}

// This will just get the block from the cache

BlockID WorldInteraction::GetBlock(Ray ray, ClientCache* cache) {
  if (cache->collusion_manager_.CheckRayIntersection(ray)) {
    BlockPos pos{floor(ray.end_point_.x), floor(ray.end_point_.y),
                 floor(ray.end_point_.z)};
    return cache->GetBlock(pos);
  }
  return context_.blocks_->AIR;
}

void WorldInteraction::BreakBlock(Ray ray, ServerInterface* interface,
                                  ClientCache* cache) {
  if (cache->collusion_manager_.CheckRayIntersection(ray)) {
    // Send block break packet
    PlayerPacket::PlayerDestroyBlock packet;
    packet.pos_ = {floor(ray.end_point_.x), floor(ray.end_point_.y),
                   floor(ray.end_point_.z)};

    interface->SendPlayerAction(packet);
  }
}

void WorldInteraction::PlaceBlock(Ray ray, BlockID block,
                                  ServerInterface* interface,
                                  ClientCache* cache) {
  if (cache->collusion_manager_.CheckRayIntersection(ray)) {
    int bounce_surface = ray.bounce_surface_;

    glm::ivec3 placePos = floor(ray.end_point_);

    placePos[bounce_surface >> 1] +=
        (bounce_surface & 0b1) -
        ((bounce_surface + 1) &
         0b1);  // Offsets block location to be placed by 1 block
    PlayerPacket::PlayerPlaceBlock packet;
    packet.block_ = block;
    packet.pos_ = {placePos.x, placePos.y, placePos.z};
    interface->SendPlayerAction(packet);
  }
}
