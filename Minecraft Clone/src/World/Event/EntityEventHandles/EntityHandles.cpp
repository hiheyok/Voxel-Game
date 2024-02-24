#include "EntityHandles.h"
#include "../../Entity/Entities.h"
#include "../../Chunk/Block/Block.h"
#include "../../World.h"
void HandleEntityTick(Event::EventDataType::_EntityEvent e) { //prob useless
	World* world = static_cast<World*>(Block::WorldPTR);
	Entity* entity = world->GetEntity(e.EntityID);
	entity->Tick();
}

void HandleEntitySummon(Event::EventDataType::_EntityEvent e) {
	World* world = static_cast<World*>(Block::WorldPTR);
	Entity entity;
	entity.Properties.Position.x = e.x;
	entity.Properties.Position.y = e.y;
	entity.Properties.Position.z = e.z;
	entity.Type = e.entityType;
	world->AddEntity(entity);
}

void HandleRemoveEntity(Event::EventDataType::_EntityEvent e) {
	World* world = static_cast<World*>(Block::WorldPTR);
	world->RemoveEntity(e.EntityID);
}