#include "LocalWorld.h"
#include <glm/geometric.hpp>
#include "../../World/Event/EventHandler.h"
#include <glm/gtx/common.hpp>
#include "../../Utils/Math/vectorOperations.h"

using namespace std;
using namespace glm;

void LocalWorld::UpdateIO(std::unordered_set<char> Keys, bool shift, float cursorx, float cursory, bool Left, bool Middle, bool Right, float delta) {

	if (Keys.count('C')) {
		enableCollusion = !enableCollusion;
	}

	sprint = Keys.count(0x01); // ctrl

	RotatePlayer(cursorx, cursory);

	MovePlayer(Keys.count('W'), Keys.count('A'), Keys.count('S'), Keys.count('D'), Keys.count(' '), shift, delta);

	WorldInteractions(Left, Middle, Right);

}

void LocalWorld::SetWorld(World* world_) {
	world = world_;
}

void LocalWorld::SetPlayerPosition(double x, double y, double z) {
	Player.Properties.Position = dvec3(x, y, z);
}

void LocalWorld::SetPlayerRotation(double x, double y) {
	Player.Properties.Rotation = dvec2(x, y);
}

vec3 LocalWorld::GetPlayerPosition() {
	return Player.Properties.Position;
}

vec2 LocalWorld::GetPlayerRotation() {
	return Player.Properties.Rotation;
}

void LocalWorld::WorldInteractions(bool Left, bool Middle, bool Right) {
	if (Right == true)
		PlaceBlock();

	if (Left == true)
		BreakBlock();

	if (Middle == true)
		GetBlock();
}

void LocalWorld::RotatePlayer(float cursorx, float cursory) {
	float CamSensitivity = 0.1f;

	Player.Properties.Rotation.x += cursorx * CamSensitivity;
	Player.Properties.Rotation.y -= cursory * CamSensitivity;

	if (Player.Properties.Rotation.y > 89.9999f)
		Player.Properties.Rotation.y = 89.9999f;
	if (Player.Properties.Rotation.y < -89.9999f)
		Player.Properties.Rotation.y = -89.9999f;

}

float velocityCurve(float current, float max, float delta) {

	int currentTime = -log(max - current) + log(max);

	int x = delta - log(max) - currentTime;

	if (x <= 0) {
		x = 0;
	}

	return max - exp(-x);
}

void LocalWorld::MovePlayer(bool KeyW, bool KeyA, bool KeyS, bool KeyD, bool KeySpace, bool shift, float delta) {

	vec3 front(
			cos(Player.Properties.Rotation.x * 0.0174533) * cos(Player.Properties.Rotation.y * 0.0174533),
			0,
			sin(Player.Properties.Rotation.x * 0.0174533) * cos(Player.Properties.Rotation.y * 0.0174533)
		);

	front = normalize(front);

	vec3 right = normalize(cross(front, glm::vec3(0.0, 1.0, 0.0)));
	right.y = 0;

	float velocity = speed;

	if (sprint) {
		velocity *= 4.f;
	}

	float v = velocityCurve(magnitude(Player.Properties.Velocity), velocity, delta) / sqrtf(2);

	Player.Properties.Velocity = vec3(0.f, 0.f, 0.f);

	if (KeyW) {
		Player.Properties.Velocity += front * v;
	}
	if (KeyA) {
		Player.Properties.Velocity += -right * v;
	}
	if (KeyS) {
		Player.Properties.Velocity += -front * v;
	}
	if (KeyD) {
		Player.Properties.Velocity += right * v;
	}

	if (shift) {
		Player.Properties.Velocity.y += -velocityCurve(magnitude(Player.Properties.Velocity), velocity, delta);
	}


	if (KeySpace && (world->IsEntityOnGround(Player) && enableCollusion)) {
		Player.Properties.Velocity.y +=  delta * 75000.f;
	}

	if (KeySpace) {
		Player.Properties.Velocity.y += velocity;
	}
	

	if (enableCollusion) {
		float gravity = -80;

		Player.Properties.Velocity.y += gravity * delta;

		vec3 time = world->GetTimeTillCollusion(Player);

		if ((time.x != -1.) && (time.x <= delta)) {
			Player.Properties.Velocity.x = 0;
		}
		if ((time.y != -1.) && (time.y <= delta)) {
			Player.Properties.Velocity.y = 0;

			Player.Properties.Velocity.x = Player.Properties.Velocity.x * powf(1.f / 250.f, delta);
			Player.Properties.Velocity.z = Player.Properties.Velocity.z * powf(1.f / 250.f, delta);
		}
		if ((time.z != -1.) && (time.z <= delta)) {
			Player.Properties.Velocity.z = 0;
		}
	}
	

	Player.Properties.Position += Player.Properties.Velocity * delta / 2.f;
	Player.Properties.Velocity = Player.Properties.Velocity * powf(3.f / 25.f, delta);

}

void LocalWorld::PlaceBlock() {
	Ray ray;

	ray.Origin = Player.Properties.Position;

	ray.Direction = dvec3(
		cos(Player.Properties.Rotation.x * 0.0174533) * cos(Player.Properties.Rotation.y * 0.0174533),
		sin(Player.Properties.Rotation.y * 0.0174533),
		sin(Player.Properties.Rotation.x * 0.0174533) * cos(Player.Properties.Rotation.y * 0.0174533));

	if (world->RayIntersection(ray)) {

		int BounceSurface = ray.bouncesurface;

		ivec3 PlacePos = floor(ray.EndPoint);

		PlacePos[(int)floor(BounceSurface / 2)] += (BounceSurface % 2) - (BounceSurface + 1) % 2; //Offsets block location to be placed by 1 block

		Event placeBlock;
		placeBlock.Type = BLOCK_EVENT;
		placeBlock.Data.BlockEvent.block = HoldingBlock;
		placeBlock.Data.BlockEvent.x = PlacePos.x;
		placeBlock.Data.BlockEvent.y = PlacePos.y;
		placeBlock.Data.BlockEvent.z = PlacePos.z;
		placeBlock.Data.BlockEvent.id = EventHandler.BlockPlace;

		world->QueueEvent(placeBlock);
	}
}

void LocalWorld::BreakBlock() {
	Ray ray;

	ray.Origin = Player.Properties.Position;

	ray.Direction = dvec3(
		cos(Player.Properties.Rotation.x * 0.0174533) * cos(Player.Properties.Rotation.y * 0.0174533),
		sin(Player.Properties.Rotation.y * 0.0174533),
		sin(Player.Properties.Rotation.x * 0.0174533) * cos(Player.Properties.Rotation.y * 0.0174533));

	if (world->RayIntersection(ray)) {
		Event placeBlock;
		placeBlock.Type = BLOCK_EVENT;
		placeBlock.Data.BlockEvent.block = Blocks.AIR;
		placeBlock.Data.BlockEvent.x = (int)floor(ray.EndPoint.x);
		placeBlock.Data.BlockEvent.y = (int)floor(ray.EndPoint.y);
		placeBlock.Data.BlockEvent.z = (int)floor(ray.EndPoint.z);
		placeBlock.Data.BlockEvent.id = EventHandler.BlockPlace;
		world->QueueEvent(placeBlock);
	}
}

void LocalWorld::GetBlock() {
	Ray ray;

	ray.Origin = Player.Properties.Position;

	ray.Direction = dvec3(
		cos(Player.Properties.Rotation.x * 0.0174533) * cos(Player.Properties.Rotation.y * 0.0174533),
		sin(Player.Properties.Rotation.y * 0.0174533),
		sin(Player.Properties.Rotation.x * 0.0174533) * cos(Player.Properties.Rotation.y * 0.0174533)
	);

	if (world->RayIntersection(ray)) {
		HoldingBlock = world->GetBlock((int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z));
	}
}