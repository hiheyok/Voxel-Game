#pragma once
#include "EntityType.h"
#include "../../Chunk/Block/Blocks.h"
#include "../../Event/EventHandler.h"
#include "../../../Level/Server/Server.h"
#include "../../../Level/World/WorldInteraction/WorldInteractions.h"
class FallingBlock : public EntityType {
	virtual void Tick(Entity* entity) override {
		Dimension* CurrentDimension = static_cast<Dimension*>(Block::DimensionPTR);
		Server* CurrentServer = static_cast<Server*>(Block::serverPTR);
	//	std::cout << entity->Properties.Velocity.y << "\n";
		float MSPT = 1.0f / (float)CurrentServer->settings.tickRate;

		//Physics

		//Logger.LogInfo("Sand", std::to_string(entity->Properties.Position.y));

		entity->Properties.Acceleration.y = -CurrentDimension->worldInteractions.settings.Gravity;

		entity->Properties.Velocity += entity->Properties.Acceleration * MSPT;

		int DistanceCheck = (int)ceil(abs(entity->Properties.Velocity.y * MSPT));

		float CollusionDistance = CurrentDimension->worldInteractions.Collusions.GetDistanceUntilCollusionSingleDirection(entity->Properties.Position, NY, DistanceCheck + 1);

		float TimeTillCollusion = abs(CollusionDistance / entity->Properties.Velocity.y);

		bool CollideWithGround = false;

		if ((TimeTillCollusion < MSPT) && (CollusionDistance != -1.f)) {
			entity->Properties.Position[1] += entity->Properties.Velocity[1] * (float)TimeTillCollusion;
			entity->Properties.Velocity[1] = 0.f;
			entity->Properties.Acceleration[1] = 0.f;

			CollideWithGround = true;
		}
		else {
			entity->Properties.Position[1] += entity->Properties.Velocity[1] * MSPT;
		}
		entity->isDirty = true;

		if (CollideWithGround) {
			Event::BlockEvent addBlock{ BlockPos{
											(int)entity->Properties.Position.x,
											(int)entity->Properties.Position.y,
											(int)entity->Properties.Position.z},
										Blocks.SAND, EventHandler.BlockPlace };
			CurrentDimension->EventManager.AddEvent(addBlock);

			Event::EntityEvent removeEntity;
			removeEntity.id_ = EventHandler.RemoveEntity;
			removeEntity.entity_uuid_= entity->Properties.EntityUUID;
			removeEntity.unique_id_ = 200;
			
			CurrentDimension->EventManager.AddEvent(removeEntity);
		}
	}
};