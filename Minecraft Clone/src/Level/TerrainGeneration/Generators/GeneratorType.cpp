#include "GeneratorType.h"

WorldGeneratorID GeneratorType::RegisterWorldGenerator(WorldGenerator* Gen) {
    generator_list_.push_back(Gen);
    g_logger.LogInfo("World Generator", "Registered Generator: " + std::to_string(generator_list_.size() - 1));
    return generator_list_.size() - 1;
}

WorldGenerator* GeneratorType::GetGenerator(WorldGeneratorID ID) {
    return generator_list_[ID];
}