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

}