#pragma once
#include <string>
#include "../../FileManager/Files.h"

struct ResourceLocation {
	const std::string NAMESPACE_DIVIDER = ":";
	std::string DEFAULT_NAMESPACE = "minecraft";
	std::string PATH = "";
	std::string NAMESPACE = "";

	std::string GetPath();

	std::string SetResourceLocation(std::string name);

	ResourceLocation() {}

	ResourceLocation(std::string name) : PATH(name) {}
};