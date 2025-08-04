// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Player/MainPlayer.h"

#include <glm/vec2.hpp>
#include <utility>

#include "Client/Inputs/InputManager.h"
#include "Client/Player/PlayerMovement.h"
#include "Client/Player/PlayerPOV.h"
#include "Client/Player/WorldInteraction.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Entity/Properties/EntityProperties.h"
#include "Level/Item/ItemTextureAtlas.h"
#include "RenderEngine/GUI/GUI.h"
#include "RenderEngine/GUI/GUISet.h"
#include "RenderEngine/Window.h"

MainPlayer::MainPlayer(GameContext& game_context, Window* window,
                       ServerInterface* interface, ClientCache* cache)
    : game_context_{game_context},
      player_{std::make_unique<Player>()},
      movement_{std::make_unique<PlayerMovement>()},
      interactions_{std::make_unique<WorldInteraction>(game_context)},
      player_pov_{std::make_unique<PlayerPOV>()},
      player_gui_{std::make_unique<GUI>(game_context, window)},
      internal_interface_{interface},
      client_cache_{cache} {
  float ItemViewRelativeSize = 0.85f;

  GUISet hotbar{game_context};
  hotbar.SetGUITexture("assets/minecraft/textures/gui/widgets.png");
  hotbar.AddGUIElement(
      "Hotbar", "",
      glm::vec2(9.f * kHotbarSize * 1.0055555555f, kHotbarSize * 1.05f),
      glm::vec2(0.f, -1.f + kHotbarSize * 0.5f), glm::vec2(0.5f, 0.5f),
      glm::vec2(181.5f, 21.5f));

  hotbar.AddGUIElement("Select", "",
                       glm::vec2(kHotbarSize * 1.1f, kHotbarSize * 1.1f),
                       glm::vec2(-kHotbarSize * 4.f, -1.f + kHotbarSize * 0.5f),
                       glm::vec2(0.5f, 22.5f), glm::vec2(22.5f, 44.5f));

  GUISet itemBar{game_context};
  itemBar.SetGUITexture(game_context_.item_atlas_->Get(),
                        game_context_.item_atlas_->GetWidth(),
                        game_context_.item_atlas_->GetHeight());

  for (int i = 0; i < kHotbarSlots; i++) {
    itemBar.AddGUIElementNorm(std::to_string(i), "",
                              glm::vec2(kHotbarSize * ItemViewRelativeSize,
                                        kHotbarSize * ItemViewRelativeSize),
                              glm::vec2(kHotbarSize * static_cast<float>(i - 4),
                                        -1.f + kHotbarSize * 0.5f),
                              glm::vec2(0, 0), glm::vec2(1, 1));
  }

  gui_index_ = player_gui_->AddGUI("PlayerGUI", std::move(hotbar));
  item_gui_index_ = player_gui_->AddGUI("Itembar", std::move(itemBar));
}

MainPlayer::~MainPlayer() = default;

PlayerPOV* MainPlayer::GetPlayerPOV() { return player_pov_.get(); }

void MainPlayer::RenderGUIs() { player_gui_->Render(); }

void MainPlayer::PrepareGUIs() {
  size_t currentSlotIndex = player_->entity_inventory_.right_hand_slot_;
  if (currentSlotIndex != slot_index_) {
    slot_index_ = currentSlotIndex;
    player_gui_->EditGUISet(gui_index_)
        .EditElementPosition(
            "Select",
            glm::vec2(kHotbarSize * (static_cast<int>(slot_index_) - 4),
                      -1.f + kHotbarSize * 0.5f));
  }

  for (int i = 0; i < kHotbarSlots; i++) {
    ItemStack item = player_->entity_inventory_.GetItem(i);
    ItemUVMapping uv =
        game_context_.item_atlas_->items_uv_map_[item.item_.properties_.id_];

    if (item.IsInitialized()) {
      player_gui_->EditGUISet(item_gui_index_)
          .EditElementUVNorm(std::to_string(i), uv.uv_1_, uv.uv_2_);
    } else {
      player_gui_->EditGUISet(item_gui_index_)
          .EditElementUVNorm(std::to_string(i), glm::vec2(0.f, 0.f),
                             glm::vec2(0.f, 0.f));
    }
  }

  player_gui_->PrepareRenderer();
}

void MainPlayer::Update(const InputManager& inputs) {
  InventoryUpdate(inputs);
  interactions_->Interact(player_.get(), inputs, internal_interface_,
                          client_cache_);
  movement_->Update(player_.get(), inputs, client_cache_);
  PrepareGUIs();

  player_pov_->SetPosition(player_->properties_.position_);
  player_pov_->SetRotation(player_->properties_.rotation_);
}

EntityProperty MainPlayer::GetEntityProperties() {
  return player_->properties_;
}

void MainPlayer::SetPlayerRotation(float x, float y) {
  player_->properties_.rotation_ = glm::vec2(x, y);
  player_pov_->SetRotation(glm::vec2(x, y));
}

void MainPlayer::SetPlayerPosition(float x, float y, float z) {
  player_->properties_.position_ = glm::vec3(x, y, z);
  player_pov_->SetPosition(glm::vec3(x, y, z));
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
