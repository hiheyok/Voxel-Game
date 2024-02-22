#pragma once

#include "../Collusion/Hitbox.h"
#include "../Properties/EntityProperties.h"
#include "../../../RenderEngine/EntityRenderer/Models/EntityModel.h"

typedef unsigned short EntityTypeID;

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

	virtual void tick(EntityProperty* entity) {

	}

	EntityTypeID ID = NULL;

	EntityModel RenderModel;

private:
	
	AABB Hitbox;
};


