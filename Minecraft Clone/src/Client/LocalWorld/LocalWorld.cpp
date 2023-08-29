#include "LocalWorld.h"

#include <glm/geometric.hpp>

#include <glm/gtx/common.hpp>

using namespace std;
using namespace glm;

void LocalWorld::UpdateIO(std::unordered_set<char> Keys, bool shift, float cursorx, float cursory, bool Left, bool Middle, bool Right, float delta) {

	if (Keys.count('C')) {
		enableCollusion = !enableCollusion;
	}

	if (Keys.count('R')) {
		speed++;
	}

	if (Keys.count('T')) {
		speed--;
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
	Player.Position = dvec3(x, y, z);
}

void LocalWorld::SetPlayerRotation(double x, double y) {
	Player.Rotation = dvec2(x, y);
}

vec3 LocalWorld::GetPlayerPosition() {
	return Player.Position;
}

vec2 LocalWorld::GetPlayerRotation() {
	return Player.Rotation;
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

	Player.Rotation.x += cursorx * CamSensitivity;
	Player.Rotation.y -= cursory * CamSensitivity;

	if (Player.Rotation.y > 89.9999f)
		Player.Rotation.y = 89.9999f;
	if (Player.Rotation.y < -89.9999f)
		Player.Rotation.y = -89.9999f;

}

void LocalWorld::MovePlayer(bool KeyW, bool KeyA, bool KeyS, bool KeyD, bool KeySpace, bool shift, float delta) {

	vec3 front(
			cos(Player.Rotation.x * 0.0174533) * cos(Player.Rotation.y * 0.0174533),
			0,
			sin(Player.Rotation.x * 0.0174533) * cos(Player.Rotation.y * 0.0174533)
		);

	front = normalize(front);

	vec3 right = normalize(cross(front, glm::vec3(0.0, 1.0, 0.0)));
	right.y = 0;

	float velocity = speed * delta;

	if (sprint) {
		velocity *= 4.f;
	}

	if (KeyW) {
		Player.Velocity += front * velocity;
	}
	if (KeyA) {
		Player.Velocity -= right * velocity;
	}
	if (KeyS) {
		Player.Velocity -= front * velocity;
	}
	if (KeyD) {
		Player.Velocity += right * velocity;
	}

	if (shift) {
		Player.Velocity.y += -velocity;
	}


	if (KeySpace && (world->IsEntityOnGround(Player) && enableCollusion)) {
		Player.Velocity.y +=  delta * 75000.f;
	}

	if (KeySpace) {
		Player.Velocity.y += velocity;
	}


	

	if (enableCollusion) {
		float gravity = 0;

		Player.Velocity.y += gravity * delta;

		vec3 time = world->GetTimeTillCollusion(Player);

		if ((time.x != -1.) && (time.x <= delta)) {
			Player.Velocity.x = 0;
		}
		if ((time.y != -1.) && (time.y <= delta)) {
			Player.Velocity.y = 0;

			Player.Velocity.x = Player.Velocity.x * powf(1.f / 250.f, delta);
			Player.Velocity.z = Player.Velocity.z * powf(1.f / 250.f, delta);
		}
		if ((time.z != -1.) && (time.z <= delta)) {
			Player.Velocity.z = 0;
		}
	}
	

	Player.Position += Player.Velocity * delta / 2.f;
	Player.Velocity = Player.Velocity * powf(3.f / 25.f, delta);



}

void LocalWorld::PlaceBlock() {
	Ray ray;

	ray.Origin = Player.Position;

	ray.Direction = dvec3(
		cos(Player.Rotation.x * 0.0174533) * cos(Player.Rotation.y * 0.0174533), 
		sin(Player.Rotation.y * 0.0174533), 
		sin(Player.Rotation.x * 0.0174533) * cos(Player.Rotation.y * 0.0174533));

	if (world->RayIntersection(ray)) {

		int BounceSurface = ray.bouncesurface;

		ivec3 PlacePos = floor(ray.EndPoint);

		PlacePos[(int)floor(BounceSurface / 2)] += (BounceSurface % 2) - (BounceSurface + 1) % 2; //Offsets block location to be placed by 1 block

		world->SetBlock(HoldingBlock, PlacePos.x, PlacePos.y, PlacePos.z);
	}
}

void LocalWorld::BreakBlock() {
	Ray ray;

	ray.Origin = Player.Position;

	ray.Direction = dvec3(
		cos(Player.Rotation.x * 0.0174533) * cos(Player.Rotation.y * 0.0174533), 
		sin(Player.Rotation.y * 0.0174533), 
		sin(Player.Rotation.x * 0.0174533) * cos(Player.Rotation.y * 0.0174533));

	if (world->RayIntersection(ray)) {
		world->SetBlock(AIR, (int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z));
	}
}

void LocalWorld::GetBlock() {
	Ray ray;

	ray.Origin = Player.Position;

	ray.Direction = dvec3(
		cos(Player.Rotation.x * 0.0174533) * cos(Player.Rotation.y * 0.0174533), 
		sin(Player.Rotation.y * 0.0174533), 
		sin(Player.Rotation.x * 0.0174533) * cos(Player.Rotation.y * 0.0174533)
	);

	if (world->RayIntersection(ray)) {
		HoldingBlock = world->GetBlock((int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z));
	}
}