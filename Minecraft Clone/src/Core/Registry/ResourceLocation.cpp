// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Core/Registry/ResourceLocation.h"

#include <string>
#include <utility>

#include "FileManager/Files.h"
#include "Utils/LogUtils.h"

using std::string;

ResourceLocation::ResourceLocation() = default;
ResourceLocation::ResourceLocation(string location, string prefix) {
  // Search for namespace
  size_t idx = location.find(':');

  if (idx == string::npos) {
    namespace_ = kDefaultNamespace;
    location_ = location;
  } else {
    namespace_ = location.substr(0, idx);
    location_ = location.substr(idx + 1);
  }

  prefix_ = prefix;
}

ResourceLocation::~ResourceLocation() = default;

ResourceLocation::ResourceLocation(ResourceLocation&& other) noexcept = default;
ResourceLocation::ResourceLocation(const ResourceLocation&) = default;

string ResourceLocation::GetPath() const {
  string full_path = string(kAssetPath) + "/";
  full_path += namespace_ + "/";
  full_path += prefix_;
  full_path += location_;
  return full_path;
}

string ResourceLocation::ToString() const {
  string out = namespace_ + ":" + prefix_ + location_;
  return out;
}

bool ResourceLocation::operator==(const ResourceLocation& other) const {
  return GetPath() == other.GetPath();
}

ResourceLocation& ResourceLocation::operator=(const ResourceLocation&) =
    default;
ResourceLocation& ResourceLocation::operator=(ResourceLocation&&) noexcept =
    default;
