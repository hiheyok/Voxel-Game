#include "Client/ECS/ClientECSManager.h"

#include <memory>

#include "Client/ECS/ClientEntitySystems.h"

ClientECSManager::ClientECSManager(GameContext& context, WorldInterface& world)
    : systems_{std::make_unique<ClientEntitySystems>()} {}

ClientECSManager::~ClientECSManager() = default;

// IECSManager interface
IEntitySystems& ClientECSManager::GetSystems() { return *systems_; }

const IEntitySystems& ClientECSManager::GetSystems() const { return *systems_; }

void ClientECSManager::CommitAll() { systems_->CommitAll(); }

// Client-specific access to concrete type
ClientEntitySystems& ClientECSManager::GetConcreteSystems() noexcept {
  return *systems_;
}

const ClientEntitySystems& ClientECSManager::GetConcreteSystems()
    const noexcept {
  return *systems_;
}
