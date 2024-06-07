#include "GeneratorType.h"

using namespace std;

WorldGeneratorID GeneratorType::RegisterWorldGenerator(WorldGenerator* Gen) {
	GeneratorList.push_back(Gen);
	Logger.LogInfo("World Generator", "Registered Generator: " + to_string(GeneratorList.size() - 1));
	return GeneratorList.size() - 1;
}

WorldGenerator* GeneratorType::GetGenerator(WorldGeneratorID ID) {
	return GeneratorList[ID];
}