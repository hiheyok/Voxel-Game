#pragma once

class IEntitySystems;

/**
 * Interface for ECS managers - shared by server and client.
 * System access is delegated to IEntitySystems via GetSystems().
 */
class IECSManager {
 public:
  virtual ~IECSManager() = default;

  // Access to systems container - all system access goes through here
  virtual IEntitySystems& GetSystems() = 0;
  virtual const IEntitySystems& GetSystems() const = 0;

  // Convenience method that delegates to GetSystems().CommitAll()
  virtual void CommitAll() = 0;
};
