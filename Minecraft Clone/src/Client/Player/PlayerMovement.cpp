#include "PlayerMovement.h"

void PlayerMovement::Update(Player* player, UserInputs Inputs, InternalServer* server) {

	if (Inputs.CheckKeyPress(KEY_C)) {
		m_EnableCollusion = !m_EnableCollusion;
	}

	RotatePlayer(player, Inputs);
	MovePlayer(player, Inputs, server);
}

float PlayerMovement::velocityMovementCurve(float current, float max, float delta) {

	int currentTime = -log(max - current) + log(max);

	int x = delta - log(max) - currentTime;

	if (x <= 0) {
		x = 0;
	}

	return max - exp(-x);
}

void PlayerMovement::RotatePlayer(Player* player, UserInputs Inputs) {
	float CamSensitivity = 0.1f;

	player->Properties.Rotation.x += Inputs.Mouse.Displacement.x * CamSensitivity;
	player->Properties.Rotation.y -= Inputs.Mouse.Displacement.y * CamSensitivity;

	if (player->Properties.Rotation.y > 89.9999f)
		player->Properties.Rotation.y = 89.9999f;
	if (player->Properties.Rotation.y < -89.9999f)
		player->Properties.Rotation.y = -89.9999f;
}

void PlayerMovement::MoveRelative(Player* player, float strafe, float up, float forward,  float friction) {
	float f = strafe * strafe + up * up + forward * forward;

	if (f >= 1.0e-4f) {
		f = sqrtf(f);

		f = f < 1.f ? 1.0 : f;
		
		f = friction / f;
		strafe = strafe * f;
		up = up * f;
		forward = forward * f;

		float zCoord = sin(player->Properties.Rotation.y * 0.017453292f);
		float xCoord = cos(player->Properties.Rotation.y * 0.017453292f);

		player->Properties.Velocity.x += strafe * xCoord - forward * zCoord;
		player->Properties.Velocity.y += up;
		player->Properties.Velocity.z += forward * xCoord + strafe * zCoord;
	}
}

void PlayerMovement::MovePlayer(Player* player, UserInputs Inputs, InternalServer* server) {

	vec3 front(
		cos(player->Properties.Rotation.x * 0.0174533) * cos(player->Properties.Rotation.y * 0.0174533),
		0,
		sin(player->Properties.Rotation.x * 0.0174533) * cos(player->Properties.Rotation.y * 0.0174533)
	);

	front = normalize(front);

	vec3 right = normalize(cross(front, glm::vec3(0.0, 1.0, 0.0)));
	right.y = 0;

	float velocity = player->Properties.MaxSpeed;

	if (Inputs.CheckKey(KEY_LEFT_CONTROL)) {
		velocity *= 8.f;
	}

	float v = velocityMovementCurve(magnitude(player->Properties.Velocity), velocity, Inputs.delta) / sqrtf(2);

	player->Properties.Velocity = vec3(0.f, 0.f, 0.f);

	if (Inputs.CheckKey(KEY_W)) {
		player->Properties.Velocity += front * v;
	}
	if (Inputs.CheckKey(KEY_A)) {
		player->Properties.Velocity += -right * v;
	}
	if (Inputs.CheckKey(KEY_S)) {
		player->Properties.Velocity += -front * v;
	}
	if (Inputs.CheckKey(KEY_D)) {
		player->Properties.Velocity += right * v;
	}

	if (Inputs.CheckKey(KEY_LEFT_SHIFT)) {
		player->Properties.Velocity.y += -velocityMovementCurve(magnitude(player->Properties.Velocity), velocity, Inputs.delta);
	}


	if (Inputs.CheckKey(KEY_SPACE) && (server->checkPlayerOnGround() && m_EnableCollusion)) {
		player->Properties.Velocity.y += velocity * 4000;
	}

	if (Inputs.CheckKey(KEY_SPACE)) {
		player->Properties.Velocity.y += velocity;
	}


	if (m_EnableCollusion) {
		float gravity = -80;

		player->Properties.Velocity.y += gravity;

		vec3 time = server->getPlayerCollusionTimes();

		if ((time.x != -1.) && (time.x <= Inputs.delta)) {
			player->Properties.Velocity.x = 0;
		}
		if ((time.y != -1.) && (time.y <= Inputs.delta)) {
			player->Properties.Velocity.y = 0;

			player->Properties.Velocity.x = player->Properties.Velocity.x * powf(1.f / 250.f, Inputs.delta);
			player->Properties.Velocity.z = player->Properties.Velocity.z * powf(1.f / 250.f, Inputs.delta);
		}
		if ((time.z != -1.) && (time.z <= Inputs.delta)) {
			player->Properties.Velocity.z = 0;
		}
	}


	player->Properties.Position += player->Properties.Velocity * Inputs.delta / 2.f;
	player->Properties.Velocity = player->Properties.Velocity * powf(3.f / 25.f, Inputs.delta);
	/*float jumpMovementFactor = 0.02F;
	float f = 0.91;

	float blockSlipperiness = 0.6;

	if (server->checkPlayerOnGround() && m_EnableCollusion) {
		f = blockSlipperiness * 0.91;
	}

	float f2 = 0.16277136F / (f * f * f);
	float friction = 0.f;

	if (server->checkPlayerOnGround() && m_EnableCollusion) {
		friction = f2 * 0.08;
	}
	else {
		friction = jumpMovementFactor;
	}

	float baseAcceleration = 0.1f;
	if (Inputs.CheckKey(KEY_LEFT_SHIFT)) {
		baseAcceleration = 0.13f;
	}
	
	float strafe = 0.f, forward = 0.f, up = 0.f;

	if (Inputs.CheckKey(KEY_W)) {
		forward += baseAcceleration;
	}
	if (Inputs.CheckKey(KEY_A)) {
		strafe -= baseAcceleration;
	}
	if (Inputs.CheckKey(KEY_S)) {
		forward -= baseAcceleration;
	}
	if (Inputs.CheckKey(KEY_D)) {
		strafe += baseAcceleration;
	}

	if (Inputs.CheckKey(KEY_SPACE)) {
		up += baseAcceleration;
	}

	MoveRelative(player , strafe, up, forward, friction);*/


}