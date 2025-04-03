#include "Core/Registry/ResourceLocation.h"
#include "FileManager/Files.h"
#include "Utils/LogUtils.h"
void ResourceLocation::SetResourceLocation(std::string name) {
    std::vector<std::string> tokens = Tokenize(name, ':');

    if (tokens.size() != 2) {
        g_logger.LogError("ResourceLocation::SetResourceLocation", "Invalid resource location");
    }

    namespace_ = tokens[0];
    path_ = tokens[1];
}

std::string ResourceLocation::GetPath() {
    return path_;
}

ResourceLocation::ResourceLocation() {}
ResourceLocation::ResourceLocation(std::string name) : path_(name) {}