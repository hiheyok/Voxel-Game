// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "FallingBlock.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Entity/Entity.h"

FallingBlock::FallingBlock(GameContext& context) : EntityType{context} {}
FallingBlock::~FallingBlock() = default;

void FallingBlock::Tick(Entity* entity, Dimension* dimension) {
  float mspt = 1.0f / static_cast<float>(dimension->tick_rate_);

  // Physics

  entity->properties_.acceleration_.y = -dimension->world_->parameters.gravity_;

  entity->properties_.velocity_ += entity->properties_.acceleration_ * mspt;

  int distanceCheck =
      static_cast<int>(ceil(abs(entity->properties_.velocity_.y * mspt)));

  float collusionDistance =
      dimension->collusion_detector_->TraceSingleAxisCollision(
          entity->properties_.position_, Directions<BlockPos>::kDown,
          distanceCheck + 1);

  float timeTillCollusion =
      abs(collusionDistance / entity->properties_.velocity_.y);

  bool collideWithGround = false;

  if ((timeTillCollusion < mspt) && (collusionDistance != -1.f)) {
    entity->properties_.position_[1] +=
        entity->properties_.velocity_[1] * timeTillCollusion;
    entity->properties_.velocity_[1] = 0.f;
    entity->properties_.acceleration_[1] = 0.f;

    collideWithGround = true;
  } else {
    entity->properties_.position_[1] += entity->properties_.velocity_[1] * mspt;
  }
  entity->is_dirty_ = true;

  if (collideWithGround) {
    BlockEvent addBlock{
        {entity->properties_.position_.x, entity->properties_.position_.y,
         entity->properties_.position_.z},
        context_.blocks_->SAND,
        context_.event_handler_->BlockPlace};
    dimension->event_manager_.AddEvent(addBlock);

    EntityEvent removeEntity;
    removeEntity.id_ = context_.event_handler_->RemoveEntity;
    removeEntity.entity_uuid_ = entity->properties_.entity_uuid_;
    removeEntity.unique_id_ = 50;

    dimension->event_manager_.AddEvent(removeEntity);
  }
}
