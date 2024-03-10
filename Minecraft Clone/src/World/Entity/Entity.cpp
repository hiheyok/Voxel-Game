#include "Entity.h"
#include "Entities.h"

void Entity::Tick() {
	EntityList.EntityTypeList[Properties.Type]->Tick(this);
}