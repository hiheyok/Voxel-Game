// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Player/WorldInteraction.h"

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "Client/ClientLevel/ClientCache.h"
#include "Client/IO/IO.h"
#include "Client/IO/KEY_CODE.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Networking/PlayerAction.h"
#include "Level/Block/Blocks.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Item/Items.h"
#include "Utils/Math/Ray/Ray.h"
#include "Utils/Math/vectorOperations.h"

WorldInteraction::WorldInteraction(GameContext& game_context)
    : game_context_{game_context} {}
WorldInteraction::~WorldInteraction() = default;

void WorldInteraction::Interact(Player* player, const UserInputs& inputs,
                                ServerInterface* interface,
                                ClientCache* cache) {
  Ray ray;
  ray.origin_ = player->properties_.position_;
  ray.direction_ =
      glm::dvec3(cos(player->properties_.rotation_.x * 0.0174533) *
                     cos(player->properties_.rotation_.y * 0.0174533),
                 sin(player->properties_.rotation_.y * 0.0174533),
                 sin(player->properties_.rotation_.x * 0.0174533) *
                     cos(player->properties_.rotation_.y * 0.0174533));

  BlockID block = player->entity_inventory_
                      .GetItem(player->entity_inventory_.right_hand_slot_)
                      .item_.GetBlock();

  if (inputs.mouse_.right_ == inputs.mouse_.PRESS) {
    PlaceBlock(ray, block, interface, cache);
  }

  if (inputs.mouse_.left_ == inputs.mouse_.PRESS) {
    BreakBlock(ray, interface, cache);
  }
  if (inputs.mouse_.middle_ == inputs.mouse_.PRESS) {
    BlockID newBlock = GetBlock(ray, cache);
    if (newBlock != game_context_.blocks_->AIR) {
      player->entity_inventory_.SetSlot(
          player->entity_inventory_.right_hand_slot_,
          ItemStack{game_context_.items_->GetItem(
              game_context_.items_->GetBlockItem(newBlock))});
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
  return game_context_.blocks_->AIR;
}

void WorldInteraction::BreakBlock(Ray ray, ServerInterface* interface,
                                  ClientCache* cache) {
  if (cache->collusion_manager_.CheckRayIntersection(ray)) {
    // Send block break packet
    PlayerPacket::PlayerDestroyBlock packet;
    packet.pos_ = BlockPos{floor(ray.end_point_.x), floor(ray.end_point_.y),
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
    packet.pos_ = BlockPos{placePos.x, placePos.y, placePos.z};
    interface->SendPlayerAction(packet);
  }
}
