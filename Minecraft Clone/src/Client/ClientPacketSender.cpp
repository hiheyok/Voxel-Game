// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientPacketSender.h"

#include "Client/Player/MainPlayer.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Networking/PlayerAction.h"
#include "Level/Entity/Mobs/Player.h"

ClientPacketSender::ClientPacketSender(MainPlayer& main_player)
    : main_player_{main_player} {}

void ClientPacketSender::SendPackets(ServerInterface* server) {
  if (server == nullptr) return;
  SendPlayerMovement(server);
}

void ClientPacketSender::SendPlayerMovement(ServerInterface* server) {
  PlayerPacket::PlayerMove packet;
  packet.acc_ = main_player_.player_->properties_.acceleration_;
  packet.vel_ = main_player_.player_->properties_.velocity_;
  packet.pos_ = main_player_.player_->properties_.position_;
  server->SendPlayerAction(packet);
}
