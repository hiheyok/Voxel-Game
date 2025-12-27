#pragma once

#include <glm/vec3.hpp>
#include <stack>

#include "Core/Typenames.h"
#include "Level/ECS/EntityType.h"

class GameContext;
class EntitySystems;

/**
 * This class handles the creation of entities
 * Mapping of the uuid to the entity types
 */

class EntityRegistry {
 public:
  explicit EntityRegistry(GameContext& context, EntitySystems& entity_system);

  EntityType GetEntityType(EntityUUID uuid) const noexcept;
  void RemoveEntity(EntityUUID uuid);
  bool IsValidUUID(EntityUUID uuid) const noexcept;

  EntityUUID CreateSand(glm::vec3 position);
  
  // Create entity with specific UUID (for syncing from server)
  void CreateEntityWithUUID(EntityUUID uuid, EntityType type);

 private:
  EntityUUID GetNewUUID(EntityType entity_type);
  void RemoveEntityUUID(EntityUUID uuid);

  GameContext& context_;
  EntitySystems& entity_systems_;
  std::vector<EntityType> uuid_id_mapping_;
  std::stack<EntityUUID> leftover_uuids_;
};