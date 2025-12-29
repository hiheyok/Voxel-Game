#pragma once

class ITransformSystem;
class ITickStateSystem;

/**
 * Base interface for entity systems containers.
 * Server and Client have different implementations with different system sets.
 */
class IEntitySystems {
 public:
  virtual ~IEntitySystems() = default;

  // Common systems available on both server and client
  virtual ITransformSystem& GetTransformSystem() = 0;
  virtual const ITransformSystem& GetTransformSystem() const = 0;
  
  virtual ITickStateSystem& GetTickStateSystem() = 0;
  virtual const ITickStateSystem& GetTickStateSystem() const = 0;

  virtual void CommitAll() = 0;
};
