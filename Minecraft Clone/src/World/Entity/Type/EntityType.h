#pragma once
#include "../Typenames.h"
#include "../Collusion/Hitbox.h"
#include "../../../RenderEngine/EntityRenderer/Models/EntityModel.h"
#include "../Entity.h"

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

	virtual void Tick(Entity* entity) {
		
	}

	EntityTypeID ID = NULL;

	EntityModel RenderModel;

	std::string EntityName;

private:
	
	AABB Hitbox;
};


