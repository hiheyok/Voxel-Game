#include "EntityHandles.h"
#include "../../Entity/Entities.h"
#include "../../Chunk/Block/Block.h"
#include "../../../Level/Dimension/Dimension.h"
void HandleEntityTick(Event::EntityEvent e) { //prob useless
	Dimension* dimension = static_cast<Dimension*>(Block::dimension_ptr_);
	Entity* entity = dimension->world_interactions_.GetEntity(e.entity_type_);
	entity->Tick();
}

void HandleEntitySummon(Event::EntityEvent e) {
	Dimension* dimension = static_cast<Dimension*>(Block::dimension_ptr_);
	Entity* entity = new Entity;
	entity->properties_.position_.x = static_cast<float>(e.pos_.x);
	entity->properties_.position_.y = static_cast<float>(e.pos_.y);
	entity->properties_.position_.z = static_cast<float>(e.pos_.z);
	entity->properties_.type_ = e.entity_type_;
	dimension->world_interactions_.AddEntity(entity);
}

void HandleRemoveEntity(Event::EntityEvent e) {
	Dimension* dimension = static_cast<Dimension*>(Block::dimension_ptr_);
	dimension->world_interactions_.KillEntity(e.entity_uuid_);
}