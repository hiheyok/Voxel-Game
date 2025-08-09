#pragma once

#include <string>
#include <vector>

#include "Assets/AssetState.h"
#include "RenderEngine/RenderResources/RenderResourceState.h"

/*
The purpose of this class is to give a universal interface for loading,
building, and interacting with assets
*/

class RenderResource {
 public:
  RenderResource(const std::string& key);
  virtual ~RenderResource();
  const std::string& GetKey() const noexcept;

  virtual void Load() = 0;

 protected:
  std::string key_;
  RenderResourceState state;
};
