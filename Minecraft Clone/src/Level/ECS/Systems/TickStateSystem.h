#pragma once

#include "Level/ECS/Components/TickStateComponent.h"
#include "Level/ECS/EntitySystem.h"
#include "Level/ECS/Systems/ITickStateSystem.h"

/**
 * This manages the status of the entity based on what chunks it is in.
 */

class TickStateSystem : public EntitySystem<TickStateComponent>, public ITickStateSystem {
 public:
  TickStateSystem(GameContext& context, WorldInterface& world,
                  ServerEntitySystems& entity_systems);

  void Tick() override;

  void SetActive(EntityUUID uuid) noexcept;
  void SetUnactive(EntityUUID uuid) noexcept;
  
  // ITickStateSystem interface
  bool HasComponent(EntityUUID uuid) const override;
  bool IsActive(EntityUUID uuid) const noexcept override;
};
