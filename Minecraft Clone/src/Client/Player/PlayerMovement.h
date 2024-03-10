#pragma once

#include "../../World/Item/Inventory/PlayerInventory.h"
#include "WorldInteraction.h"
#include "../IO/IO.h"

class PlayerMovement {
public:

	void Update(Player* player, UserInputs Inputs, World* world);

private:

	float velocityMovementCurve(float current, float max, float delta);

	void RotatePlayer(Player* player, UserInputs Inputs);

	void MovePlayer(Player* player, UserInputs Inputs, World* world);

	bool m_EnableCollusion = false;

};