#pragma once
#include "EntityType.h"
#include "../../Chunk/Block/Blocks.h"
#include "../../Event/EventHandler.h"
#include "../../../Level/Server/Server.h"
#include "../../../Level/World/WorldInteraction/WorldInteractions.h"
class FallingBlock : public EntityType {
	virtual void Tick(Entity* entity) override {
		Dimension* currentDimension = static_cast<Dimension*>(Block::dimension_ptr_);
		Server* currentServer = static_cast<Server*>(Block::server_ptr_);
	//	std::cout << entity->Properties.Velocity.y << "\n";
		float mspt = 1.0f / (float)currentServer->settings_.tick_rate_;

		//Physics

		//Logger.LogInfo("Sand", std::to_string(entity->Properties.Position.y));

		entity->properties_.acceleration_.y = -currentDimension->world_interactions_.settings_.gravity_;

		entity->properties_.velocity_ += entity->properties_.acceleration_ * mspt;

		int distanceCheck = (int)ceil(abs(entity->properties_.velocity_.y * mspt));

		float collusionDistance = currentDimension->world_interactions_.collusions_.GetDistanceUntilCollusionSingleDirection(entity->properties_.position_, NY, distanceCheck + 1);

		float timeTillCollusion = abs(collusionDistance / entity->properties_.velocity_.y);

		bool collideWithGround = false;

		if ((timeTillCollusion < mspt) && (collusionDistance != -1.f)) {
			entity->properties_.position_[1] += entity->properties_.velocity_[1] * (float)timeTillCollusion;
			entity->properties_.velocity_[1] = 0.f;
			entity->properties_.acceleration_[1] = 0.f;

			collideWithGround = true;
		}
		else {
			entity->properties_.position_[1] += entity->properties_.velocity_[1] * mspt;
		}
		entity->is_dirty_ = true;

		if (collideWithGround) {
			Event::BlockEvent addBlock{ BlockPos{
											(int)entity->properties_.position_.x,
											(int)entity->properties_.position_.y,
											(int)entity->properties_.position_.z},
										Blocks.SAND, EventHandler.BlockPlace };
			currentDimension->event_manager_.AddEvent(addBlock);

			Event::EntityEvent removeEntity;
			removeEntity.id_ = EventHandler.RemoveEntity;
			removeEntity.entity_uuid_= entity->properties_.entity_uuid_;
			removeEntity.unique_id_ = 200;
			
			currentDimension->event_manager_.AddEvent(removeEntity);
		}
	}
};