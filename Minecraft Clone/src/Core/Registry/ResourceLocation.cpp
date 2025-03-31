#include "ResourceLocation.h"
#include "../../FileManager/Files.h"

void ResourceLocation::SetResourceLocation(std::string name) {
    std::vector<std::string> tokens = Tokenize(name, ':');

    if (tokens.size() != 2) {
        throw std::exception("Invalid resource location");
    }

    namespace_ = tokens[0];
    path_ = tokens[1];
}

std::string ResourceLocation::GetPath() {
    return path_;
}

ResourceLocation::ResourceLocation() {}
ResourceLocation::ResourceLocation(std::string name) : path_(name) {}