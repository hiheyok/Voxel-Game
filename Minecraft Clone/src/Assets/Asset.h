#pragma once

#include <string>
#include <vector>

#include "Assets/AssetHandle.h"
#include "Assets/AssetState.h"
/*
The purpose of this class is to give a universal interface for loading,
building, and interacting with assets

Assets aren't copiable and movable and should be own by AssetManager
*/

class Asset {
 public:
  explicit Asset(const std::string& key);

  Asset(const Asset&) = delete;
  Asset& operator=(const Asset&) = delete;
  Asset(Asset&&) = delete;
  Asset& operator=(Asset&&) = delete;
  
  virtual ~Asset();
  const std::string& GetKey() const noexcept;

  virtual void Load() = 0;

 protected:
  std::string key_;
  AssetState state;
};
