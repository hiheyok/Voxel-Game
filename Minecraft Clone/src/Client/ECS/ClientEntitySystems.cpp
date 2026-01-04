#include "Client/ECS/ClientEntitySystems.h"

#include "Core/Typenames.h"
#include "Level/ECS/EntityType.h"
#include "Level/ECS/Systems/ITickStateSystem.h"
#include "Level/ECS/Systems/ITransformSystem.h"

// IEntitySystems interface
ITransformSystem& ClientEntitySystems::GetTransformSystem() {
  return transform_system_;
}

const ITransformSystem& ClientEntitySystems::GetTransformSystem() const {
  return transform_system_;
}

ITickStateSystem& ClientEntitySystems::GetTickStateSystem() {
  return tick_state_system_;
}

const ITickStateSystem& ClientEntitySystems::GetTickStateSystem() const {
  return tick_state_system_;
}

void ClientEntitySystems::CommitAll() {
  // No-op for client: client systems receive updates directly without
  // double-buffering
}

// Client-specific methods
void ClientEntitySystems::RegisterEntity(EntityUUID uuid, EntityType type) {
  if (uuid >= uuid_to_type_.size()) {
    uuid_to_type_.resize(uuid + 1, EntityType::kInvalidEntity);
  }
  uuid_to_type_[uuid] = type;
}

EntityType ClientEntitySystems::GetEntityType(EntityUUID uuid) const {
  if (uuid >= uuid_to_type_.size()) {
    return EntityType::kInvalidEntity;
  }
  return uuid_to_type_[uuid];
}

void ClientEntitySystems::UnregisterEntity(EntityUUID uuid) {
  if (uuid < uuid_to_type_.size()) {
    uuid_to_type_[uuid] = EntityType::kInvalidEntity;
  }
}

bool ClientEntitySystems::IsValidUUID(EntityUUID uuid) const {
  return uuid < uuid_to_type_.size() &&
         uuid_to_type_[uuid] != EntityType::kInvalidEntity;
}
