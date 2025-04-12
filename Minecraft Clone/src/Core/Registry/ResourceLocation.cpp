#include "Core/Registry/ResourceLocation.h"
#include "FileManager/Files.h"
#include "Utils/LogUtils.h"
ResourceLocation::ResourceLocation() = default;
ResourceLocation::ResourceLocation(std::string path, std::string namespaceIn) {
    if (namespaceIn == "") namespaceIn = std::string(kDefaultNamespace);
    path_ = std::string(kAssetPath) + '/' + namespaceIn + '/' + path;
}

ResourceLocation::~ResourceLocation() = default;

ResourceLocation::ResourceLocation(ResourceLocation&& other) noexcept {
    *this = std::move(other);
}
ResourceLocation::ResourceLocation(const ResourceLocation&) = default;

void ResourceLocation::SetPath(std::string path, std::string namespaceIn) {
    path_ = std::string(kAssetPath) + '/' + namespaceIn + '/' + path;
}

std::string ResourceLocation::GetPath() const {
    return path_;
}

bool ResourceLocation::operator==(const ResourceLocation& other) const {
    return GetPath() == other.GetPath();
}

ResourceLocation& ResourceLocation::operator=(const ResourceLocation&) = default;
ResourceLocation& ResourceLocation::operator=(ResourceLocation&&) noexcept = default;