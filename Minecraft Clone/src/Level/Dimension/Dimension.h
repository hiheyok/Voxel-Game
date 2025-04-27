#pragma once
#include "Level/Dimension/DimensionProperties.h"
#include "Level/Container/EntityContainer.h"
#include "Level/Event/EventSystem.h"
#include "Level/Event/EventHandler.h"
#include "Level/TerrainGeneration/ChunkGenerator.h"
#include "Level/TerrainGeneration/Generators/GeneratorType.h"
#include "Level/World/World.h"
#include "Level/World/WorldParameters.h"
#include "Core/Options/Option.h"

class WorldInterface;
class CollusionDetector;
class WorldUpdater;
class ChunkGenerator;
class LightEngine;

class Dimension {
public:
    EventSystem event_manager_;
    const int tick_rate_ = 20; //TODO: tmp fix

    Dimension(DimensionProperties properties, WorldGeneratorID generatorTypeIn = g_generators.DEBUG);
    virtual ~Dimension();

    virtual void Tick() = 0;

    WorldGeneratorID GetGeneratorType() const;
    bool CheckTickUsed(EventID id, const BlockPos& pos) const;
    void TickUsed(EventID id, const BlockPos& pos);
    void EventTick();
    void Update();

    WorldInterface* world_;
    std::unique_ptr<WorldUpdater> world_updater_;
    std::unique_ptr<CollusionDetector> collusion_detector_;
private:
    std::unique_ptr<World> main_world_;
    std::unique_ptr<ChunkGenerator> chunk_generator_;
    std::unique_ptr<LightEngine> light_engine_;

    WorldGeneratorID generator_type_;
    DimensionProperties properties_;
    WorldParameters world_settings_;

    FastHashMap<int, FastHashSet<BlockPos>> tick_usage_;
};