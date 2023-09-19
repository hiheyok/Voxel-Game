#pragma once
#include "../../World/Entity/Entity.h"
#include "../../World/World.h"

class LocalWorld {
public:

	LocalWorld() {
		Player.Type = PLAYER;
	}

	void UpdateIO(std::unordered_set<char> Keys, bool shift, float cursorx, float cursory, bool Left, bool Middle, bool Right, float delta);

	void SetWorld(World* world_);

	void SetPlayerPosition(double x, double y, double z);

	void SetPlayerRotation(double x, double y);

	glm::vec3 GetPlayerPosition();

	glm::vec2 GetPlayerRotation();

private:

	void WorldInteractions(bool Left, bool Middle, bool Right);

	void RotatePlayer(float cursorx, float cursory);

	void MovePlayer(bool KeyW, bool KeyA, bool KeyS, bool KeyD, bool KeySpace, bool shift, float delta);

	void PlaceBlock();

	void BreakBlock();

	void GetBlock();

	BlockID HoldingBlock = WHITE_CONCRETE;
	Entity Player;
	World* world = nullptr;

	bool enableCollusion = true;
	bool sprint = false;
	float speed = 50.f;
};

