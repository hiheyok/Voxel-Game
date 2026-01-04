// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientPacketSender.h"

#include <variant>

#include "Client/ClientActionQueue.h"
#include "Client/Player/MainPlayer.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Networking/PlayerAction.h"
#include "Level/Entity/Mobs/Player.h"

ClientPacketSender::ClientPacketSender(ServerInterface& server,
                                       MainPlayer& main_player)
    : server_{server}, main_player_{main_player} {}

void ClientPacketSender::ProcessQueue(const ClientActionQueue& queue) {
  for (const auto& action : queue.GetActions()) {
    std::visit(
        [this](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, BlockDestroyAction>) {
            HandleBlockDestroy(arg.pos);
          } else if constexpr (std::is_same_v<T, BlockPlaceAction>) {
            HandleBlockPlace(arg.pos, arg.block);
          } else if constexpr (std::is_same_v<T, PlayerMoveAction>) {
            HandlePlayerMove(arg.pos, arg.vel, arg.acc);
          }
        },
        action);
  }
}

void ClientPacketSender::SendPackets() { SendPlayerMovement(); }

void ClientPacketSender::SendPlayerMovement() {
  PlayerPacket::PlayerMove packet;
  packet.acc_ = main_player_.player_->properties_.acceleration_;
  packet.vel_ = main_player_.player_->properties_.velocity_;
  packet.pos_ = main_player_.player_->properties_.position_;
  server_.SendPlayerAction(packet);
}

void ClientPacketSender::HandleBlockDestroy(BlockPos pos) {
  PlayerPacket::PlayerDestroyBlock packet;
  packet.pos_ = pos;
  server_.SendPlayerAction(packet);
}

void ClientPacketSender::HandleBlockPlace(BlockPos pos, BlockID block) {
  PlayerPacket::PlayerPlaceBlock packet;
  packet.pos_ = pos;
  packet.block_ = block;
  server_.SendPlayerAction(packet);
}

void ClientPacketSender::HandlePlayerMove(glm::vec3 pos, glm::vec3 vel,
                                          glm::vec3 acc) {
  PlayerPacket::PlayerMove packet;
  packet.pos_ = pos;
  packet.vel_ = vel;
  packet.acc_ = acc;
  server_.SendPlayerAction(packet);
}
