#include "MainPlayer.h"

void MainPlayer::Initialize(GLFWwindow* win) {
	PlayerGUI.Initialize(win);

	GUISet Hotbar;
	Hotbar.SetGUITexture("assets/textures/gui/widgets.png");
	Hotbar.AddGUIElement("Hotbar", "", vec2(1.35f * 1.2f, 0.15f * 1.2f), vec2(0.f, -0.85f), vec2(1.f, 1.f), vec2(181.f, 21.f));
	Hotbar.AddGUIElement("Select", "", vec2(0.15f * 1.2f, 0.15f * 1.2f), vec2(0.15f * 1.2f * (-4.f), -0.85f), vec2(1.f, 23.f), vec2(22.f, 44.f));

	GUIIndex = PlayerGUI.AddGUI("PlayerGUI", Hotbar);
}

void MainPlayer::RenderGUIs() {
	PlayerGUI.Render();
}

void MainPlayer::PrepareGUIs() {
	int CurrentSlotIndex = m_Player.m_EntityInventory.RightHandSlot;
	if (CurrentSlotIndex != SlotIndex) {
		SlotIndex = CurrentSlotIndex;
		PlayerGUI.EditGUISet(GUIIndex).EditElementPosition("Select", vec2(0.15f * 1.2f * (float)(SlotIndex - 4), -0.85f));
	}

	PlayerGUI.PrepareRenderer();
}

void MainPlayer::Update(UserInputs Inputs) {
	InventoryUpdate(Inputs);
	m_Interactions.Interact(&m_Player, Inputs);
	m_Movement.Update(&m_Player, Inputs, static_cast<World*>(Block::WorldPTR));
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
		Direction = -1;
		break;
	case Inputs.Mouse.SCROLL_UP:
		Direction = 1;
		break;
	case Inputs.Mouse.SCROLL_NONE:
		break;
	}

	int CurrentInventorySlot = m_Player.m_EntityInventory.RightHandSlot;
	int MaxInventorySize = m_Player.m_EntityInventory.GetSlotCount();

	if (CurrentInventorySlot + Direction == MaxInventorySize) {
		Direction = 0;
	}

	if (CurrentInventorySlot + Direction == -1) {
		Direction = 0;
	}

	CurrentInventorySlot += Direction;

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