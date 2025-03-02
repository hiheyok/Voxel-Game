#include "Entity.h"
#include "Entities.h"

void Entity::Tick() {
	g_entity_list.entity_type_list_[properties_.type_]->Tick(this);
}