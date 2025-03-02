#include "GeneratorType.h"

using namespace std;

WorldGeneratorID GeneratorType::RegisterWorldGenerator(WorldGenerator* Gen) {
	generator_list_.push_back(Gen);
	Logger.LogInfo("World Generator", "Registered Generator: " + to_string(generator_list_.size() - 1));
	return generator_list_.size() - 1;
}

WorldGenerator* GeneratorType::GetGenerator(WorldGeneratorID ID) {
	return generator_list_[ID];
}