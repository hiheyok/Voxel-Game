// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Player/MainPlayer.h"

#include <memory>

#include "Client/ClientActionQueue.h"
#include "Client/Inputs/InputManager.h"
#include "Client/Inputs/MouseInputs.h"
#include "Client/Player/PlayerMovement.h"
#include "Client/Player/PlayerPOV.h"
#include "Client/Player/WorldInteraction.h"
#include "Core/GameContext/GameContext.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Entity/Properties/EntityProperties.h"

using std::make_unique;

MainPlayer::MainPlayer(GameContext& context, ClientCache& cache,
                       UIManager& ui_manager)
    : context_{context},
      player_{make_unique<Player>()},
      movement_{make_unique<PlayerMovement>()},
      interactions_{make_unique<WorldInteraction>(context)},
      player_pov_{make_unique<PlayerPOV>()},
      ui_manager_{ui_manager},
      client_cache_{cache} {
  // Set main player pointer in context for UI access
  context_.main_player_ = player_.get();
}

MainPlayer::~MainPlayer() {
  // Clear the context pointer on destruction
  context_.main_player_ = nullptr;
}

PlayerPOV* MainPlayer::GetPlayerPOV() { return player_pov_.get(); }

void MainPlayer::Update(const InputManager& inputs,
                        ClientActionQueue& action_queue) {
  InventoryUpdate(inputs);
  interactions_->Interact(*player_, inputs, action_queue, client_cache_);
  movement_->Update(*player_, inputs, client_cache_);

  player_pov_->SetPosition(player_->properties_.position_);
  player_pov_->SetRotation(player_->properties_.rotation_);
}

EntityProperty MainPlayer::GetEntityProperties() {
  return player_->properties_;
}

void MainPlayer::SetPlayerRotation(float x, float y) {
  player_->properties_.rotation_ = {x, y};
  player_pov_->SetRotation({x, y});
}

void MainPlayer::SetPlayerPosition(float x, float y, float z) {
  player_->properties_.position_ = {x, y, z};
  player_pov_->SetPosition({x, y, z});
}

void MainPlayer::InventoryUpdate(const InputManager& inputs) {
  int direction = 0;
  switch (inputs.GetScrollState()) {
    case MouseInputs::ScrollState::kDown: {
      direction = 1;
      break;
    }
    case MouseInputs::ScrollState::kUp: {
      direction = -1;
      break;
    }
    case MouseInputs::ScrollState::kNone: {
      break;
    }
  }

  int currentInventorySlot = player_->entity_inventory_.right_hand_slot_;

  if (currentInventorySlot + direction == 9) {
    currentInventorySlot = 0;
  } else if (currentInventorySlot + direction == -1) {
    currentInventorySlot = kHotbarSlots - 1;
  } else {
    currentInventorySlot += direction;
  }

  if (inputs.CheckAction(InputAction::kHotbarSlot1)) currentInventorySlot = 0;
  if (inputs.CheckAction(InputAction::kHotbarSlot2)) currentInventorySlot = 1;
  if (inputs.CheckAction(InputAction::kHotbarSlot3)) currentInventorySlot = 2;
  if (inputs.CheckAction(InputAction::kHotbarSlot4)) currentInventorySlot = 3;
  if (inputs.CheckAction(InputAction::kHotbarSlot5)) currentInventorySlot = 4;
  if (inputs.CheckAction(InputAction::kHotbarSlot6)) currentInventorySlot = 5;
  if (inputs.CheckAction(InputAction::kHotbarSlot7)) currentInventorySlot = 6;
  if (inputs.CheckAction(InputAction::kHotbarSlot8)) currentInventorySlot = 7;
  if (inputs.CheckAction(InputAction::kHotbarSlot9)) currentInventorySlot = 8;

  player_->entity_inventory_.right_hand_slot_ = currentInventorySlot;
}
