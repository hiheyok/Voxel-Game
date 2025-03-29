#include "MainPlayer.h"
#include "../../Level/Item/ItemTextureAtlas.h"

void MainPlayer::Initialize(GLFWwindow* win, InternalServer* server) {
    player_gui_.Initialize(win);

    float ItemViewRelativeSize = 0.85f;

    GUISet hotbar;
    hotbar.SetGUITexture("assets/minecraft/textures/gui/widgets.png");
    hotbar.AddGUIElement("Hotbar", "",
        glm::vec2(9.f * hotbar_size_ * 1.0055555555f, hotbar_size_ * 1.05f),
        glm::vec2(0.f, -1.f + hotbar_size_ * 0.5f),
        glm::vec2(0.5f, 0.5f),
        glm::vec2(181.5f,21.5f));

    hotbar.AddGUIElement("Select", "",
        glm::vec2(hotbar_size_ * 1.1f,  hotbar_size_ * 1.1f),
        glm::vec2(-hotbar_size_ * 4.f, -1.f + hotbar_size_ * 0.5f),
        glm::vec2(0.5f, 22.5f),
        glm::vec2(22.5f, 44.5f));

    GUISet itemBar;
    itemBar.SetGUITexture(g_item_atlas.atlas_.texture_id_,
        g_item_atlas.atlas_.width_,
        g_item_atlas.atlas_.height_);

    for (int i = 0; i < 9; i++) {
        itemBar.AddGUIElementNorm(std::to_string(i), "",
            glm::vec2(hotbar_size_* ItemViewRelativeSize, hotbar_size_ * ItemViewRelativeSize),
            glm::vec2(hotbar_size_ * (float)(i - 4), -1.f + hotbar_size_ * 0.5f),
            glm::vec2(0,0),
            glm::vec2(1,1));
    }

    gui_index_ = player_gui_.AddGUI("PlayerGUI", hotbar);
    item_gui_index_ = player_gui_.AddGUI("Itembar", itemBar);
    internal_server_ = server;
}

void MainPlayer::RenderGUIs() {
    player_gui_.Render();
}

void MainPlayer::PrepareGUIs() {
    size_t currentSlotIndex = player_.entity_inventory_.right_hand_slot_;
    if (currentSlotIndex != slot_index_) {
        slot_index_ = currentSlotIndex;
        player_gui_.EditGUISet(gui_index_).EditElementPosition("Select", glm::vec2(hotbar_size_ * (float)((int)slot_index_ - 4), -1.f + hotbar_size_ * 0.5f));
    }

    for (int i = 0; i < 9; i++) {
        ItemStack item = player_.entity_inventory_.GetItem(i);
        ItemUVMapping uv = g_item_atlas.items_uv_map_[item.item_.properties_.id_];

        if (item.IsInitialized()) {
            player_gui_.EditGUISet(item_gui_index_).EditElementUVNorm(std::to_string(i), uv.uv_1_, uv.uv_2_);
        }
        else {
            player_gui_.EditGUISet(item_gui_index_).EditElementUVNorm(std::to_string(i), glm::vec2(0.f,0.f), glm::vec2(0.f,0.f));
        }
        

    }

    player_gui_.PrepareRenderer();
}

void MainPlayer::Update(UserInputs inputs, Dimension* dimension) {
    InventoryUpdate(inputs);
    interactions_.Interact(&player_, inputs, dimension);
    movement_.Update(&player_, inputs, internal_server_);
    PrepareGUIs();
}

EntityProperty MainPlayer::GetEntityProperties() {
    return player_.properties_;
}

void MainPlayer::SetPlayerRotation(float x, float y) {
    player_.properties_.rotation_ = glm::vec2(x, y);
}

void MainPlayer::SetPlayerPosition(float x, float y, float z) {
    player_.properties_.position_ = glm::vec3(x, y, z);
}

void MainPlayer::InventoryUpdate(UserInputs inputs) {
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

    int currentInventorySlot = player_.entity_inventory_.right_hand_slot_;
    // int MaxInventorySize = m_Player.m_EntityInventory.GetSlotCount();

    if (currentInventorySlot + Direction == 9) {
        currentInventorySlot = 0;
    } else if (currentInventorySlot + Direction == -1) {
        currentInventorySlot = 8;
    }
    else {
        currentInventorySlot += Direction;
    }

    if (inputs.CheckKeyPress(KEY_1))
        currentInventorySlot = 0;
    if (inputs.CheckKeyPress(KEY_2))
        currentInventorySlot = 1;
    if (inputs.CheckKeyPress(KEY_3))
        currentInventorySlot = 2;
    if (inputs.CheckKeyPress(KEY_4))
        currentInventorySlot = 3;
    if (inputs.CheckKeyPress(KEY_5))
        currentInventorySlot = 4;
    if (inputs.CheckKeyPress(KEY_6))
        currentInventorySlot = 5;
    if (inputs.CheckKeyPress(KEY_7))
        currentInventorySlot = 6;
    if (inputs.CheckKeyPress(KEY_8))
        currentInventorySlot = 7;
    if (inputs.CheckKeyPress(KEY_9))
        currentInventorySlot = 8;

    player_.entity_inventory_.right_hand_slot_ = currentInventorySlot;
}