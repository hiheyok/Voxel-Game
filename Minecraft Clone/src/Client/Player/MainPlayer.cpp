#include "MainPlayer.h"
#include "../../World/Item/ItemTextureAtlas.h"

void MainPlayer::Initialize(GLFWwindow* win, InternalServer* server) {
	PlayerGUI.Initialize(win);

	float ItemViewRelativeSize = 0.85f;

	GUISet Hotbar;
	Hotbar.SetGUITexture("assets/textures/gui/widgets.png");
	Hotbar.AddGUIElement("Hotbar", "", vec2(9.f * HotbarSize * 1.0055555555f, HotbarSize * 1.05f), vec2(0.f, -1.f + HotbarSize * 0.5f), vec2(0.5f, 0.5f), vec2(181.5f, 21.5f));
	Hotbar.AddGUIElement("Select", "", vec2(HotbarSize * 1.1f,  HotbarSize * 1.1f), vec2(-HotbarSize * 4.f, -1.f + HotbarSize * 0.5f), vec2(0.5f, 22.5f), vec2(22.5f, 44.5f));

	GUISet ItemBar;
	ItemBar.SetGUITexture(ItemAtlas.Atlas.textureID, ItemAtlas.Atlas.width, ItemAtlas.Atlas.height);

	for (int i = 0; i < 9; i++) {
		ItemBar.AddGUIElementNorm(std::to_string(i), "", vec2(HotbarSize* ItemViewRelativeSize, HotbarSize * ItemViewRelativeSize), vec2(HotbarSize * (float)(i - 4), -1.f + HotbarSize * 0.5f), vec2(0,0), vec2(1,1));
	}

	GUIIndex = PlayerGUI.AddGUI("PlayerGUI", Hotbar);

	ItemGUIIndex = PlayerGUI.AddGUI("Itembar", ItemBar);
	internalServer = server;
}

void MainPlayer::RenderGUIs() {
	PlayerGUI.Render();
}

void MainPlayer::PrepareGUIs() {
	int CurrentSlotIndex = m_Player.m_EntityInventory.RightHandSlot;
	if (CurrentSlotIndex != SlotIndex) {
		SlotIndex = CurrentSlotIndex;
		PlayerGUI.EditGUISet(GUIIndex).EditElementPosition("Select", vec2(HotbarSize * (float)(SlotIndex - 4), -1.f + HotbarSize * 0.5f));
	}

	for (int i = 0; i < 9; i++) {
		ItemStack item = m_Player.m_EntityInventory.GetItem(i);
		ItemUVMapping uv = ItemAtlas.ItemsUVMap[item.m_item.Properties.ID];

		if (item.IsInitialized()) {
			PlayerGUI.EditGUISet(ItemGUIIndex).EditElementUVNorm(std::to_string(i), uv.UV_1, uv.UV_2);
		}
		else {
			PlayerGUI.EditGUISet(ItemGUIIndex).EditElementUVNorm(std::to_string(i), vec2(0.f,0.f), vec2(0.f,0.f));
		}
		

	}

	PlayerGUI.PrepareRenderer();
}

void MainPlayer::Update(UserInputs Inputs) {
	InventoryUpdate(Inputs);
	m_Interactions.Interact(&m_Player, Inputs);
	m_Movement.Update(&m_Player, Inputs, internalServer);
	PrepareGUIs();
}

EntityProperty MainPlayer::GetEntityProperties() {
	return m_Player.Properties;
}

void MainPlayer::SetPlayerRotation(float x, float y) {
	m_Player.Properties.Rotation = glm::vec2(x, y);
}

void MainPlayer::SetPlayerPosition(float x, float y, float z) {
	m_Player.Properties.Position = glm::vec3(x, y, z);
}

void MainPlayer::InventoryUpdate(UserInputs Inputs) {
	int Direction = 0;

	switch (Inputs.Mouse.ScrollDirection) {
	case Inputs.Mouse.SCROLL_DOWN:
		Direction = 1;
		break;
	case Inputs.Mouse.SCROLL_UP:
		Direction = -1;
		break;
	case Inputs.Mouse.SCROLL_NONE:
		break;
	}

	int CurrentInventorySlot = m_Player.m_EntityInventory.RightHandSlot;
	int MaxInventorySize = m_Player.m_EntityInventory.GetSlotCount();

	if (CurrentInventorySlot + Direction == 9) {
		CurrentInventorySlot = 0;
	} else if (CurrentInventorySlot + Direction == -1) {
		CurrentInventorySlot = 8;
	}
	else {
		CurrentInventorySlot += Direction;
	}

	if (Inputs.CheckKeyPress(KEY_1))
		CurrentInventorySlot = 0;
	if (Inputs.CheckKeyPress(KEY_2))
		CurrentInventorySlot = 1;
	if (Inputs.CheckKeyPress(KEY_3))
		CurrentInventorySlot = 2;
	if (Inputs.CheckKeyPress(KEY_4))
		CurrentInventorySlot = 3;
	if (Inputs.CheckKeyPress(KEY_5))
		CurrentInventorySlot = 4;
	if (Inputs.CheckKeyPress(KEY_6))
		CurrentInventorySlot = 5;
	if (Inputs.CheckKeyPress(KEY_7))
		CurrentInventorySlot = 6;
	if (Inputs.CheckKeyPress(KEY_8))
		CurrentInventorySlot = 7;
	if (Inputs.CheckKeyPress(KEY_9))
		CurrentInventorySlot = 8;

	m_Player.m_EntityInventory.RightHandSlot = CurrentInventorySlot;
}