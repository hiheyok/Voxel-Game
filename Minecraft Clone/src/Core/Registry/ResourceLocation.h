#pragma once
#include <string>
#include "../../FileManager/Files.h"

struct ResourceLocation {
	const std::string NAMESPACE_DIVIDER = ":";
	std::string DEFAULT_NAMESPACE = "minecraft";
	std::string PATH = "";
	std::string NAMESPACE = "";

	std::string GetPath() {
		return PATH;
	}

	std::string SetResourceLocation(std::string name) {
		std::vector<std::string> Tokens = Tokenize(name, ':');
		
		if (Tokens.size() != 2) {
			throw std::exception("Invalid resource location");
		}

		NAMESPACE = Tokens[0];
		PATH = Tokens[1];
	}
};