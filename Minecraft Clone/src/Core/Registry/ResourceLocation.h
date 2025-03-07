#pragma once
#include <string>
#include "../../FileManager/Files.h"

struct ResourceLocation {
    const std::string namespace_divider_ = ":";
    std::string default_namespace_ = "minecraft";
    std::string path_ = "";
    std::string namespace_ = "";

    std::string GetPath();
    void SetResourceLocation(std::string name);

    ResourceLocation();
    ResourceLocation(std::string name);
};