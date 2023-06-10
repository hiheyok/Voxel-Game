#pragma once

#include "Hitbox.h"

typedef unsigned short EntityID;

class EntityType {
public:
	
	EntityType() {

	}
	
	void ChangeHitboxSize(glm::vec3 size) {
		Hitbox.size = size;
	}

	void ChangeHitboxSize(float x, float y, float z) {
		Hitbox.size = glm::vec3(x, y, z);
	}

	AABB GetHitbox() {
		return Hitbox;
	}

	EntityID ID;

private:

	AABB Hitbox;
};


