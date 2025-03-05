#pragma once
#include "../../Level/Item/Inventory/PlayerInventory.h"
#include "WorldInteraction.h"
#include "PlayerMovement.h"
#include "../IO/IO.h"
#include "../../Level/Entity/Mobs/Player.h"
#include "../../RenderEngine/GUI/GUI.h"

class MainPlayer {
public:

    void Initialize(GLFWwindow* win, InternalServer* server);

    void Update(UserInputs inputs);

    EntityProperty GetEntityProperties();

    void SetPlayerRotation(float x, float y);
    void SetPlayerPosition(float x, float y, float z);

    void RenderGUIs();

    Player player_;
private:

    void PrepareGUIs();

    void InventoryUpdate(UserInputs inputs);

    PlayerMovement movement_;
    WorldInteraction interactions_;
    InternalServer* internal_server_ = nullptr;

    GUI player_gui_;

    size_t gui_index_ = 0;
    size_t item_gui_index_ = 0;
    size_t slot_index_ = 0;

    float hotbar_size_ = 0.135;
};