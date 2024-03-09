#pragma once

#include "../../World/Item/Inventory/PlayerInventory.h"
#include "WorldInteraction.h"
#include "../IO/IO.h"

class PlayerMovement {
public:

	void Update(Entity* Player, UserInputs Inputs, World* world) {
		RotatePlayer(Player, Inputs);
		MovePlayer(Player, Inputs, world);
	}

	

private:

	float velocityMovementCurve(float current, float max, float delta) {

		int currentTime = -log(max - current) + log(max);

		int x = delta - log(max) - currentTime;

		if (x <= 0) {
			x = 0;
		}

		return max - exp(-x);
	}

	void RotatePlayer(Entity* Player, UserInputs Inputs) {
		float CamSensitivity = 0.1f;

		Player->Properties.Rotation.x += Inputs.Mouse.Displacement.x * CamSensitivity;
		Player->Properties.Rotation.y -= Inputs.Mouse.Displacement.y * CamSensitivity;

		if (Player->Properties.Rotation.y > 89.9999f)
			Player->Properties.Rotation.y = 89.9999f;
		if (Player->Properties.Rotation.y < -89.9999f)
			Player->Properties.Rotation.y = -89.9999f;
	}

	void MovePlayer(Entity* Player, UserInputs Inputs, World* world) {

		vec3 front(
			cos(Player->Properties.Rotation.x * 0.0174533) * cos(Player->Properties.Rotation.y * 0.0174533),
			0,
			sin(Player->Properties.Rotation.x * 0.0174533) * cos(Player->Properties.Rotation.y * 0.0174533)
		);

		front = normalize(front);

		vec3 right = normalize(cross(front, glm::vec3(0.0, 1.0, 0.0)));
		right.y = 0;

		float velocity = Player->Properties.MaxSpeed;

		if (Inputs.CheckKey(KEY_LEFT_CONTROL)) {
			velocity *= 4.f;
		}

		float v = velocityMovementCurve(magnitude(Player->Properties.Velocity), velocity, Inputs.delta) / sqrtf(2);

		Player->Properties.Velocity = vec3(0.f, 0.f, 0.f);

		if (Inputs.CheckKey(KEY_W)) {
			Player->Properties.Velocity += front * v;
		}
		if (Inputs.CheckKey(KEY_A)) {
			Player->Properties.Velocity += -right * v;
		}
		if (Inputs.CheckKey(KEY_S)) {
			Player->Properties.Velocity += -front * v;
		}
		if (Inputs.CheckKey(KEY_D)) {
			Player->Properties.Velocity += right * v;
		}

		if (Inputs.CheckKey(KEY_LEFT_SHIFT)) {
			Player->Properties.Velocity.y += -velocityMovementCurve(magnitude(Player->Properties.Velocity), velocity, Inputs.delta);
		}


		if (Inputs.CheckKey(KEY_SPACE) && (world->IsEntityOnGround(*Player) && enableCollusion)) {
			Player->Properties.Velocity.y += Inputs.delta * 75000.f;
		}

		if (Inputs.CheckKey(KEY_SPACE)) {
			Player->Properties.Velocity.y += velocity;
		}


		if (enableCollusion) {
			float gravity = -80;

			Player->Properties.Velocity.y += gravity * Inputs.delta;

			vec3 time = world->GetTimeTillCollusion(*Player);

			if ((time.x != -1.) && (time.x <= Inputs.delta)) {
				Player->Properties.Velocity.x = 0;
			}
			if ((time.y != -1.) && (time.y <= Inputs.delta)) {
				Player->Properties.Velocity.y = 0;

				Player->Properties.Velocity.x = Player->Properties.Velocity.x * powf(1.f / 250.f, Inputs.delta);
				Player->Properties.Velocity.z = Player->Properties.Velocity.z * powf(1.f / 250.f, Inputs.delta);
			}
			if ((time.z != -1.) && (time.z <= Inputs.delta)) {
				Player->Properties.Velocity.z = 0;
			}
		}


		Player->Properties.Position += Player->Properties.Velocity * Inputs.delta / 2.f;
		Player->Properties.Velocity = Player->Properties.Velocity * powf(3.f / 25.f, Inputs.delta);

	}

	bool enableCollusion = false;

};