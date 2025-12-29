#pragma once

#include <vector>

#include "Client/ECS/Systems/ClientTickStateSystem.h"
#include "Client/ECS/Systems/ClientTransformSystem.h"
#include "Core/Typenames.h"
#include "Level/ECS/EntityType.h"
#include "Level/ECS/IEntitySystems.h"

class GameContext;
class WorldInterface;

/**
 * Client-side entity systems container.
 * Contains only Transform and TickState systems for rendering.
 * No physics/collision systems - those are server-only.
 */
class ClientEntitySystems : public IEntitySystems {
 public:
  ClientEntitySystems() = default;
  ~ClientEntitySystems() = default;

  // IEntitySystems interface
  ITransformSystem& GetTransformSystem() override;
  const ITransformSystem& GetTransformSystem() const override;
  ITickStateSystem& GetTickStateSystem() override;
  const ITickStateSystem& GetTickStateSystem() const override;
  void CommitAll() override;

  // Client-specific: track entity types for rendering
  void RegisterEntity(EntityUUID uuid, EntityType type);
  void UnregisterEntity(EntityUUID uuid);
  EntityType GetEntityType(EntityUUID uuid) const;
  bool IsValidUUID(EntityUUID uuid) const;

 private:
  ClientTransformSystem transform_system_;
  ClientTickStateSystem tick_state_system_;
  std::vector<EntityType> uuid_to_type_;  // Indexed by UUID (sequential integers)
};
