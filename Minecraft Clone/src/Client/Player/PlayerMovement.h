#pragma once

#include "../../Level/Item/Inventory/PlayerInventory.h"
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

	void MoveRelative(Player* player, float strafe, float up, float forward, float friction);

	bool m_EnableCollusion = false;

};