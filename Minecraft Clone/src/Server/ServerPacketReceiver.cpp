// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Server/ServerPacketReceiver.h"

#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/ClientInterface.h"
#include "Core/Networking/EntityUpdate.h"
#include "Core/Networking/Packet.h"
#include "Core/Networking/PlayerAction.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Entity/Entity.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Event/Event.h"
#include "Level/Level.h"

using std::get;
using std::vector;

ServerPacketReceiver::ServerPacketReceiver(Level& level, GameContext& context)
    : level_{level}, context_{context} {}

void ServerPacketReceiver::ProcessPackets(ClientInterface* client) {
  if (client == nullptr) return;
  ProcessPlayerPackets(client);
}

void ServerPacketReceiver::ProcessPlayerPackets(ClientInterface* client) {
  vector<Packet::PlayerAction> player_packets;
  client->PollClientPlayerAction(player_packets);

  for (const auto& packet : player_packets) {
    switch (packet.type_) {
      case PlayerPacket::PacketType::GET_ITEM:
        // Send packet to player
        {
          const PlayerPacket::PlayerGetItem& get_item =
              get<PlayerPacket::PlayerGetItem>(packet.packet_);
          Entity* e =
              level_.main_world_->world_->GetEntity(client->GetPlayerUUID());
          Player* player = static_cast<Player*>(e);
          player->entity_inventory_.SetSlot(get_item.slot_, get_item.item_);

          EntityUpdatePacket::EntityInventoryUpdate inventory_update;
          inventory_update.item_ = get_item.item_;
          inventory_update.slot_ = get_item.slot_;
          inventory_update.uuid = client->GetPlayerUUID();
          client->SendEntityUpdate(inventory_update);
        }
        break;
      case PlayerPacket::PacketType::DESTROY_BLOCK: {
        const PlayerPacket::PlayerDestroyBlock& destroyBlock =
            get<PlayerPacket::PlayerDestroyBlock>(packet.packet_);
        BlockEvent block_event;
        block_event.block_ = context_.blocks_->AIR;
        block_event.id_ = context_.event_handler_->BlockPlace;
        block_event.pos_ = destroyBlock.pos_;
        level_.main_world_->event_manager_.AddEvent(block_event);
      } break;
      case PlayerPacket::PacketType::PLACE_BLOCK: {
        const PlayerPacket::PlayerPlaceBlock& placeBlock =
            get<PlayerPacket::PlayerPlaceBlock>(packet.packet_);
        BlockEvent block_event;
        block_event.block_ = placeBlock.block_;
        block_event.id_ = context_.event_handler_->BlockPlace;
        block_event.pos_ = placeBlock.pos_;
        level_.main_world_->event_manager_.AddEvent(block_event);
      } break;
      case PlayerPacket::PacketType::MOVE: {
        const PlayerPacket::PlayerMove& movePlayer =
            get<PlayerPacket::PlayerMove>(packet.packet_);
        Entity* e =
            level_.main_world_->world_->GetEntity(client->GetPlayerUUID());
        e->properties_.acceleration_ = movePlayer.acc_;
        e->properties_.velocity_ = movePlayer.vel_;
        e->properties_.position_ = movePlayer.pos_;
      } break;
    }
  }
}
