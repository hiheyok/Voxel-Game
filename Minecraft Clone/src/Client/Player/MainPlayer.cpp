#include "Client/Player/MainPlayer.h"

#include <glm/vec2.hpp>

#include "Client/IO/IO.h"
#include "Client/IO/KEY_CODE.h"
#include "Client/Player/PlayerMovement.h"
#include "Client/Player/PlayerPOV.h"
#include "Client/Player/WorldInteraction.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Entity/Properties/EntityProperties.h"
#include "Level/Item/ItemTextureAtlas.h"
#include "RenderEngine/GUI/GUI.h"
#include "RenderEngine/GUI/GUISet.h"
#include "RenderEngine/Window.h"

MainPlayer::MainPlayer(Window* window, ServerInterface* interface,
                       ClientCache* cache)
    : player_{std::make_unique<Player>()},
      movement_{std::make_unique<PlayerMovement>()},
      interactions_{std::make_unique<WorldInteraction>()},
      player_pov_{std::make_unique<PlayerPOV>()},
      player_gui_{std::make_unique<GUI>(window)},
      internal_interface_{interface},
      client_cache_{cache} {
    float ItemViewRelativeSize = 0.85f;

    GUISet hotbar;
    hotbar.SetGUITexture("assets/minecraft/textures/gui/widgets.png");
    hotbar.AddGUIElement(
        "Hotbar", "",
        glm::vec2(9.f * kHotbarSize * 1.0055555555f, kHotbarSize * 1.05f),
        glm::vec2(0.f, -1.f + kHotbarSize * 0.5f), glm::vec2(0.5f, 0.5f),
        glm::vec2(181.5f, 21.5f));

    hotbar.AddGUIElement(
        "Select", "", glm::vec2(kHotbarSize * 1.1f, kHotbarSize * 1.1f),
        glm::vec2(-kHotbarSize * 4.f, -1.f + kHotbarSize * 0.5f),
        glm::vec2(0.5f, 22.5f), glm::vec2(22.5f, 44.5f));

    GUISet itemBar;
    itemBar.SetGUITexture(g_item_atlas.Get(), g_item_atlas.GetWidth(),
                          g_item_atlas.GetHeight());

    for (int i = 0; i < 9; i++) {
        itemBar.AddGUIElementNorm(
            std::to_string(i), "",
            glm::vec2(kHotbarSize * ItemViewRelativeSize,
                      kHotbarSize * ItemViewRelativeSize),
            glm::vec2(kHotbarSize * (float)(i - 4), -1.f + kHotbarSize * 0.5f),
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
                "Select", glm::vec2(kHotbarSize * (float)((int)slot_index_ - 4),
                                    -1.f + kHotbarSize * 0.5f));
    }

    for (int i = 0; i < 9; i++) {
        ItemStack item = player_->entity_inventory_.GetItem(i);
        ItemUVMapping uv =
            g_item_atlas.items_uv_map_[item.item_.properties_.id_];

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

void MainPlayer::Update(const UserInputs& inputs) {
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

void MainPlayer::InventoryUpdate(const UserInputs& inputs) {
    int Direction = 0;

    switch (inputs.mouse_.scroll_direction_) {
        case inputs.mouse_.SCROLL_DOWN:
            Direction = 1;
            break;
        case inputs.mouse_.SCROLL_UP:
            Direction = -1;
            break;
        case inputs.mouse_.SCROLL_NONE:
            break;
    }

    int currentInventorySlot = player_->entity_inventory_.right_hand_slot_;

    if (currentInventorySlot + Direction == 9) {
        currentInventorySlot = 0;
    } else if (currentInventorySlot + Direction == -1) {
        currentInventorySlot = 8;
    } else {
        currentInventorySlot += Direction;
    }

    if (inputs.CheckKeyPress(KEY_1)) currentInventorySlot = 0;
    if (inputs.CheckKeyPress(KEY_2)) currentInventorySlot = 1;
    if (inputs.CheckKeyPress(KEY_3)) currentInventorySlot = 2;
    if (inputs.CheckKeyPress(KEY_4)) currentInventorySlot = 3;
    if (inputs.CheckKeyPress(KEY_5)) currentInventorySlot = 4;
    if (inputs.CheckKeyPress(KEY_6)) currentInventorySlot = 5;
    if (inputs.CheckKeyPress(KEY_7)) currentInventorySlot = 6;
    if (inputs.CheckKeyPress(KEY_8)) currentInventorySlot = 7;
    if (inputs.CheckKeyPress(KEY_9)) currentInventorySlot = 8;

    player_->entity_inventory_.right_hand_slot_ = currentInventorySlot;
}