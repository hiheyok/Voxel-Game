#include "EntityHandles.h"
#include "../../Entity/Entities.h"
#include "../../Chunk/Block/Block.h"
#include "../../../Level/Dimension/Dimension.h"
void HandleEntityTick(Event::EntityEvent e) { //prob useless
	Dimension* dimension = static_cast<Dimension*>(Block::DimensionPTR);
	Entity* entity = dimension->worldInteractions.getEntity(e.entity_type_);
	entity->Tick();
}

void HandleEntitySummon(Event::EntityEvent e) {
	Dimension* dimension = static_cast<Dimension*>(Block::DimensionPTR);
	Entity* entity = new Entity;
	entity->Properties.Position.x = e.pos_.x;
	entity->Properties.Position.y = e.pos_.y;
	entity->Properties.Position.z = e.pos_.z;
	entity->Properties.Type = e.entity_type_;
	dimension->worldInteractions.addEntity(entity);
}

void HandleRemoveEntity(Event::EntityEvent e) {
	Dimension* dimension = static_cast<Dimension*>(Block::DimensionPTR);
	dimension->worldInteractions.killEntity(e.entity_uuid_);
}