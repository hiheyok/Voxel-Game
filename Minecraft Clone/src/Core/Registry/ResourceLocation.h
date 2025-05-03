// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <string>
#include <string_view>

// Class used to store resource locations
class ResourceLocation {
 public:
  ResourceLocation();
  ResourceLocation(std::string path,
                   std::string namespaceIn = std::string(kDefaultNamespace));
  ~ResourceLocation();
  ResourceLocation(ResourceLocation&&) noexcept;
  ResourceLocation(const ResourceLocation&);

  void SetPath(std::string path,
               std::string namespaceIn = std::string(kDefaultNamespace));
  std::string GetPath() const;
  bool operator==(const ResourceLocation&) const;
  ResourceLocation& operator=(const ResourceLocation&);
  ResourceLocation& operator=(ResourceLocation&&) noexcept;

 private:
  static constexpr std::string_view kNamespaceDivider = ":";
  static constexpr std::string_view kDefaultNamespace = "minecraft";
  static constexpr std::string_view kAssetPath = "./assets";

  std::string path_ = "";
};

namespace std {
template <>
struct hash<ResourceLocation> {
  size_t operator()(const ResourceLocation& obj) const {
    return hash<string>{}(obj.GetPath());
  }
};
}  // namespace std
