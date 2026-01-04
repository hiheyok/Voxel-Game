// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientActionQueue.h"

void ClientActionQueue::QueueBlockDestroy(BlockPos pos) {
  actions_.push_back(BlockDestroyAction{pos});
}

void ClientActionQueue::QueueBlockPlace(BlockPos pos, BlockID block) {
  actions_.push_back(BlockPlaceAction{pos, block});
}

void ClientActionQueue::QueuePlayerMove(glm::vec3 pos, glm::vec3 vel,
                                        glm::vec3 acc) {
  actions_.push_back(PlayerMoveAction{pos, vel, acc});
}

const std::vector<ClientAction>& ClientActionQueue::GetActions() const {
  return actions_;
}

void ClientActionQueue::Clear() { actions_.clear(); }
