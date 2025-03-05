#pragma once

#include "../../Level/Item/Inventory/PlayerInventory.h"
#include "../../Level/Server/Communication/InternalServer.h"
#include "WorldInteraction.h"
#include "../IO/IO.h"

class PlayerMovement {
public:
	void Update(Player* player, const UserInputs& inputs, InternalServer* world);
private:
	float VelocityMovementCurve(float current, float max, float delta);

	void RotatePlayer(Player* player, const UserInputs& inputs);
	void MovePlayer(Player* player, const UserInputs& inputs, InternalServer* server);
	void MoveRelative(Player* player, float strafe, float up, float forward, float friction);

	bool enable_collusion_ = false;

};