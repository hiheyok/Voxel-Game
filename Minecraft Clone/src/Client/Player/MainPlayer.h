#pragma once
#include "../../World/Item/Inventory/PlayerInventory.h"
#include "WorldInteraction.h"
#include "PlayerMovement.h"
#include "../IO/IO.h"

class MainPlayer {
public:

	void Update(UserInputs Inputs) {
		InventoryUpdate(Inputs.Mouse);
		m_Interactions.Interact(&Player, Inputs);
		Movement.Update(&Player, Inputs, static_cast<World*>(Block::WorldPTR));
	}

	EntityProperty GetEntityProperties() {
		return Player.Properties;
	}

	void SetPlayerRotation(float x, float y) {
		Player.Properties.Rotation = glm::vec2(x, y);
	}

	void SetPlayerPosition(float x, float y, float z) {
		Player.Properties.Position = glm::vec3(x, y, z);
	}

private:

	void InventoryUpdate(MouseInputs Inputs) {
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

		int CurrentInventorySlot = Player.m_EntityInventory.RightHandSlot;
		int MaxInventorySize = Player.m_EntityInventory.GetSlotCount();

		if (CurrentInventorySlot + Direction == MaxInventorySize) {
			return;
		}

		if (CurrentInventorySlot + Direction == -1) {
			return;
		}

		CurrentInventorySlot += Direction;

		Player.m_EntityInventory.RightHandSlot = CurrentInventorySlot;
	}

	PlayerMovement Movement;
	WorldInteraction m_Interactions;

	Entity Player;

};