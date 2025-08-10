#pragma once

#include <string>
#include <vector>

#include "Assets/AssetState.h"
#include "RenderEngine/RenderResources/RenderResourceState.h"

/*
The purpose of this class is to give a universal interface for loading,
building, and interacting with render resources

RenderResources aren't copiable and movable and should be owned only by
RenderResourceManager
*/

class RenderResource {
 public:
  explicit RenderResource(const std::string& key);
  virtual ~RenderResource();

  RenderResource(const RenderResource&) = delete;
  RenderResource& operator=(const RenderResource&) = delete;
  RenderResource(RenderResource&&) = delete;
  RenderResource& operator=(RenderResource&&) = delete;

  const std::string& GetKey() const noexcept;

  virtual void Load() = 0;

 protected:
  std::string key_;
  RenderResourceState state;
};
