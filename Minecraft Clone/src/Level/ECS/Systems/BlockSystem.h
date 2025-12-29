#pragma once

#include "Level/ECS/Components/BlockComponent.h"
#include "Level/ECS/EntitySystem.h"

/**
 * Manages block components for entities
 */

class BlockSystem : public EntitySystem<BlockComponent> {
 public:
  BlockSystem(GameContext& context, WorldInterface& world, ServerEntitySystems& entity_systems);
  void Tick() override;
};
