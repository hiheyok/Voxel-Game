#include "MainPlayer.h"

void MainPlayer::Update(UserInputs Inputs) {
	InventoryUpdate(Inputs.Mouse);
	m_Interactions.Interact(&m_Player, Inputs);
	m_Movement.Update(&m_Player, Inputs, static_cast<World*>(Block::WorldPTR));
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

void MainPlayer::InventoryUpdate(MouseInputs Inputs) {
	int Direction = 0;

	switch (Inputs.ScrollDirection) {
	case Inputs.SCROLL_DOWN:
		Direction = -1;
		break;
	case Inputs.SCROLL_UP:
		Direction = 1;
		break;
	case Inputs.SCROLL_NONE:
		break;
	}

	if (Direction == 0) {
		return;
	}

	int CurrentInventorySlot = m_Player.m_EntityInventory.RightHandSlot;
	int MaxInventorySize = m_Player.m_EntityInventory.GetSlotCount();

	if (CurrentInventorySlot + Direction == MaxInventorySize) {
		return;
	}

	if (CurrentInventorySlot + Direction == -1) {
		return;
	}

	CurrentInventorySlot += Direction;

	m_Player.m_EntityInventory.RightHandSlot = CurrentInventorySlot;
}