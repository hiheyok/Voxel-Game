#include "Level/Entity/Entity.h"

#include "Level/Entity/Entities.h"

Entity::Entity() {}

void Entity::Tick(Dimension* dimension) {
  g_entity_list.entity_type_list_[properties_.type_]->Tick(this, dimension);
}
