#pragma once

#include "../../World/Item/Inventory/PlayerInventory.h"
#include "../../Level/Server/Communication/InternalServer.h"
#include "WorldInteraction.h"
#include "../IO/IO.h"

class PlayerMovement {
public:

	void Update(Player* player, UserInputs Inputs, InternalServer* world);

private:

	float velocityMovementCurve(float current, float max, float delta);

	void RotatePlayer(Player* player, UserInputs Inputs);

	void MovePlayer(Player* player, UserInputs Inputs, InternalServer* server);

	bool m_EnableCollusion = false;

};