// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Core/Registry/ResourceLocation.h"

#include <string>
#include <utility>

#include "FileManager/Files.h"
#include "Utils/LogUtils.h"
ResourceLocation::ResourceLocation() = default;
ResourceLocation::ResourceLocation(std::string path, std::string namespaceIn) {
  if (namespaceIn == "") namespaceIn = std::string(kDefaultNamespace);
  path_ = std::string(kAssetPath) + '/' + namespaceIn + '/' + path;
}

ResourceLocation::~ResourceLocation() = default;

ResourceLocation::ResourceLocation(ResourceLocation&& other) noexcept = default;
ResourceLocation::ResourceLocation(const ResourceLocation&) = default;

void ResourceLocation::SetPath(std::string path, std::string namespaceIn) {
  path_ = std::string(kAssetPath) + '/' + namespaceIn + '/' + path;
}

std::string ResourceLocation::GetPath() const { return path_; }

bool ResourceLocation::operator==(const ResourceLocation& other) const {
  return GetPath() == other.GetPath();
}

ResourceLocation& ResourceLocation::operator=(const ResourceLocation&) =
    default;
ResourceLocation& ResourceLocation::operator=(ResourceLocation&&) noexcept =
    default;
