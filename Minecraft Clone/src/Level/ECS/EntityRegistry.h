#pragma once

#include <glm/vec3.hpp>
#include <stack>
#include <vector>

#include "Core/Typenames.h"
#include "Level/ECS/EntityType.h"

class GameContext;
class ServerEntitySystems;

/**
 * This class handles the creation of entities
 * Mapping of the uuid to the entity types
 */

class EntityRegistry {
 public:
  explicit EntityRegistry(GameContext& context, ServerEntitySystems& entity_system);

  EntityType GetEntityType(EntityUUID uuid) const noexcept;
  void RemoveEntity(EntityUUID uuid);
  bool IsValidUUID(EntityUUID uuid) const noexcept;
  
  // Apply all pending removals
  void Commit();
  
  // Get entities pending removal (for network sync before ResetState clears them)
  const std::vector<EntityUUID>& GetRemovedEntities() const noexcept;
  
  // Clear pending removals list (call after network sync has sent them)
  void ResetState();

  EntityUUID CreateSand(glm::vec3 position);
  
  // Create entity with specific UUID (for syncing from server)
  void CreateEntityWithUUID(EntityUUID uuid, EntityType type);

 private:
  EntityUUID GetNewUUID(EntityType entity_type);
  void RemoveEntityUUID(EntityUUID uuid);

  GameContext& context_;
  ServerEntitySystems& entity_systems_;
  std::vector<EntityType> uuid_id_mapping_;
  std::stack<EntityUUID> leftover_uuids_;
  std::vector<EntityUUID> delete_queue_;  // Deferred UUID removals
};
