#pragma once
#include "../../World/Item/Inventory/PlayerInventory.h"
#include "WorldInteraction.h"
#include "PlayerMovement.h"
#include "../IO/IO.h"
#include "../../World/Entity/Mobs/Player.h"

class MainPlayer {
public:

	void Update(UserInputs Inputs);

	EntityProperty GetEntityProperties();

	void SetPlayerRotation(float x, float y);

	void SetPlayerPosition(float x, float y, float z);

private:

	void InventoryUpdate(MouseInputs Inputs);

	PlayerMovement m_Movement;
	WorldInteraction m_Interactions;
	Player m_Player;

};