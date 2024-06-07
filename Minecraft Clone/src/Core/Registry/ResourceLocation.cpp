#include "ResourceLocation.h"

std::string ResourceLocation::SetResourceLocation(std::string name) {
	std::vector<std::string> Tokens = Tokenize(name, ':');

	if (Tokens.size() != 2) {
		throw std::exception("Invalid resource location");
	}

	NAMESPACE = Tokens[0];
	PATH = Tokens[1];
}

std::string ResourceLocation::GetPath() {
	return PATH;
}