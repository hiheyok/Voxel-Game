#include "Entity.h"
#include "Entities.h"

void Entity::Tick() {
	EntityList.EntityTypeList[Type]->tick(&Properties);
}