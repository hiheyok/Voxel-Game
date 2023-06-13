#include "LocalWorld.h"

#include <glm/geometric.hpp>

#include <glm/gtx/common.hpp>

using namespace std;
using namespace glm;

void LocalWorld::UpdateIO(std::unordered_set<char> Keys, bool shift, float cursorx, float cursory, bool Left, bool Middle, bool Right, float delta) {

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

	float velocity = 50;

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

	if (KeySpace) {
		Player.Velocity.y += velocity;
	}

	if (shift) {
		Player.Velocity.y += -velocity;
	}


	//if (KeySpace && world->IsEntityOnGround(Player)) {
	//	Player.Velocity.y += 6 ;
	//}


	//float gravity = -50 * delta;        

	//Player.Velocity.y += gravity;

	//float friction = 3.0;

	//if (world->IsEntityOnGround(Player)) {
	//	friction = 1000.0F;
	//}

	vec3 time = world->GetTimeTillCollusion(Player);

	if ((time.x != -1.) && (time.x <= delta)) {
		Player.Velocity.x = 0;
	}
	if ((time.y != -1.) && (time.y <= delta)) {
		Player.Velocity.y = 0;
	}
	if ((time.z != -1.) && (time.z <= delta)) {
		Player.Velocity.z = 0;
	}

	Player.Position += Player.Velocity * delta;
	Player.Velocity = Player.Velocity * powf(1 / 5, delta);


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

		PlacePos[floor(BounceSurface / 2)] -= -(BounceSurface - (int)floor(BounceSurface / 2) * 2) + 1;

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
		if (ray.bouncesurface == NX) {
			world->SetBlock(AIR, (int)floor(ray.EndPoint.x - 1), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z));
		}
		if (ray.bouncesurface == NY) {
			world->SetBlock(AIR, (int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y - 1), (int)floor(ray.EndPoint.z));
		}
		if (ray.bouncesurface == NZ) {
			world->SetBlock(AIR, (int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z - 1));
		}
		if ((ray.bouncesurface == PX) || (ray.bouncesurface == PY) || (ray.bouncesurface == PZ)) {
			world->SetBlock(AIR, (int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z));
		}
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
		if (ray.bouncesurface == NX) {
			HoldingBlock = world->GetBlock((int)floor(ray.EndPoint.x - 1), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z));
		}
		if (ray.bouncesurface == NY) {
			HoldingBlock = world->GetBlock((int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y - 1), (int)floor(ray.EndPoint.z));
		}
		if (ray.bouncesurface == NZ) {
			HoldingBlock = world->GetBlock((int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z - 1));
		}
		if ((ray.bouncesurface == PX) || (ray.bouncesurface == PY) || (ray.bouncesurface == PZ)) {
			HoldingBlock = world->GetBlock((int)floor(ray.EndPoint.x), (int)floor(ray.EndPoint.y), (int)floor(ray.EndPoint.z));
		}

	}
}