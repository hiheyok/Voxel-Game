#pragma once
#include "EntityType.h"
#include "../../Chunk/Block/Blocks.h"
#include "../../World.h"
#include "../../Server/Server.h"
#include "../../Event/EventHandler.h"



class FallingBlock : public EntityType {
	virtual void Tick(Entity* entity) override {
		World* CurrentWorld = static_cast<World*>(Block::WorldPTR);
	//	std::cout << entity->Properties.Velocity.y << "\n";
		float MSPT = 1 / TPS;

		//Physics

		entity->Properties.Acceleration.y = -CurrentWorld->Properties.Gravity;

		entity->Properties.Velocity += entity->Properties.Acceleration * MSPT;

		glm::dvec3 TimeTillCollusion = CurrentWorld->GetTimeTillCollusion(*entity);

		bool CollideWithGround = false;

		if (TimeTillCollusion[1] > MSPT) {
			entity->Properties.Position[1] += entity->Properties.Velocity[1] * (float)TimeTillCollusion[1];
			entity->Properties.Velocity[1] = 0.f;
			entity->Properties.Acceleration[1] = 0.f;

			CollideWithGround = true;
		}
		else {
			entity->Properties.Position[1] += entity->Properties.Velocity[1] * MSPT;
		}

		entity->isDirty = true;

		if (CollideWithGround) {
			Event addBlock;
			addBlock.Type = BLOCK_EVENT;
			addBlock.Data.BlockEvent.id = EventHandler.BlockPlace;
			addBlock.Data.BlockEvent.x = (int)entity->Properties.Position.x;
			addBlock.Data.BlockEvent.y = (int)entity->Properties.Position.y;
			addBlock.Data.BlockEvent.z = (int)entity->Properties.Position.z;
			addBlock.Data.BlockEvent.block = Blocks.SAND;
			CurrentWorld->QueueEvent(addBlock);

			Event removeEntity;
			removeEntity.Type = ENTITY_EVENT;
			removeEntity.Data.EntityEvent.id = EventHandler.RemoveEntity;
			removeEntity.Data.EntityEvent.EntityID = entity->EntityUUID;
			CurrentWorld->QueueEvent(removeEntity);
		}
	}
};