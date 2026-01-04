// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientActionQueue.h"

#include <glm/vec3.hpp>
#include <vector>

#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"

using glm::vec3;
using std::vector;

void ClientActionQueue::QueueBlockDestroy(BlockPos pos) {
  actions_.push_back(BlockDestroyAction{pos});
}

void ClientActionQueue::QueueBlockPlace(BlockPos pos, BlockID block) {
  actions_.push_back(BlockPlaceAction{pos, block});
}

void ClientActionQueue::QueuePlayerMove(vec3 pos, vec3 vel, vec3 acc) {
  actions_.push_back(PlayerMoveAction{pos, vel, acc});
}

const vector<ClientAction>& ClientActionQueue::GetActions() const {
  return actions_;
}

void ClientActionQueue::Clear() { actions_.clear(); }
