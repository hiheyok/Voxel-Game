#include "EntityHandles.h"
#include "../../Entity/Entities.h"
#include "../../Chunk/Block/Block.h"
#include "../../../Level/Dimension/Dimension.h"
void HandleEntityTick(Event::EventDataType::_EntityEvent e) { //prob useless
	Dimension* dimension = static_cast<Dimension*>(Block::DimensionPTR);
	Entity* entity = dimension->worldInteractions.getEntity(e.EntityID);
	entity->Tick();
}

void HandleEntitySummon(Event::EventDataType::_EntityEvent e) {
	Dimension* dimension = static_cast<Dimension*>(Block::DimensionPTR);
	Entity* entity = new Entity;
	entity->Properties.Position.x = e.x;
	entity->Properties.Position.y = e.y;
	entity->Properties.Position.z = e.z;
	entity->Properties.Type = e.entityType;
	dimension->worldInteractions.addEntity(entity);
}

void HandleRemoveEntity(Event::EventDataType::_EntityEvent e) {
	Dimension* dimension = static_cast<Dimension*>(Block::DimensionPTR);
	dimension->worldInteractions.killEntity(e.EntityID);
}