#pragma once

#include <concepts>
#include <fstream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "Assets/AssetHandle.h"
#include "Assets/AssetState.h"
/*
The purpose of this class is to give a universal interface for loading,
building, and interacting with assets
*/

class Asset {
 public:
  Asset(const std::string& asset_key);
  virtual ~Asset();
  const std::string& GetAssetKey() const noexcept;

  virtual void Load() = 0;

 protected:

  std::string asset_key_;
  AssetState state;
};
