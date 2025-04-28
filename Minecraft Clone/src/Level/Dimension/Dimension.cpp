#include "Level/Dimension/Dimension.h"

#include "Level/Light/LightEngine.h"
#include "Level/TerrainGeneration/ChunkGenerator.h"
#include "Level/World/WorldUpdater.h"

Dimension::Dimension(DimensionProperties properties,
                     WorldGeneratorID generatorType)
    : properties_{properties},
      generator_type_{generatorType},
      main_world_{std::make_unique<World>()},
      chunk_generator_{std::make_unique<ChunkGenerator>()},
      light_engine_{std::make_unique<LightEngine>()} {
    world_settings_.horizontal_ticking_distance_ =
        g_app_options.horizontal_render_distance_;
    world_settings_.vertical_ticking_distance_ =
        g_app_options.vertical_render_distance_;

    world_ = static_cast<WorldInterface*>(main_world_.get());
    world_updater_ =
        std::make_unique<WorldUpdater>(main_world_.get(), world_settings_);
    collusion_detector_ =
        std::make_unique<CollusionDetector>(main_world_->GetChunkMap());

    if (g_generators.GetGenerator(generator_type_)->use_tall_chunks_) {
        world_updater_->tall_generation_ = true;
    }

    light_engine_->Start(g_app_options.light_engine_threads_, world_);
    chunk_generator_->Start(g_app_options.world_gen_threads_, kWorldSeed,
                            generatorType);
}
Dimension::~Dimension() = default;

WorldGeneratorID Dimension::GetGeneratorType() const { return generator_type_; }

bool Dimension::CheckTickUsed(EventID id,
                              const BlockPos& pos) const {  // temp sol
    const auto& it = tick_usage_.find(id);
    if (it == tick_usage_.end()) return false;
    return it->second.contains(pos);
}

void Dimension::TickUsed(EventID id, const BlockPos& pos) {  // temp solution
    tick_usage_[id].insert(pos);
}

void Dimension::EventTick() {
    std::unique_ptr<std::vector<Event>>& EventQueue = event_manager_.GetQueue();

    for (size_t i = 0; i < EventQueue->size(); i++) {
        const Event& e = EventQueue->at(i);
        switch (e.type_) {
            case NULL_EVENT:
                break;
            case BLOCK_EVENT: {
                const BlockEvent& blockEvent =
                    std::get<BlockEvent>(e.event_data_);
                if (CheckTickUsed(blockEvent.id_, blockEvent.pos_)) {
                    continue;
                }
                TickUsed(blockEvent.id_, blockEvent.pos_);
                break;
            }
            case ENTITY_EVENT: {
                const EntityEvent& entityEvent =
                    std::get<EntityEvent>(e.event_data_);
                if ((entityEvent.unique_id_ != 0) &&
                    CheckTickUsed(entityEvent.unique_id_, entityEvent.pos_)) {
                    continue;
                }
                // TODO: tmp fix rework this later
                if ((entityEvent.unique_id_ != 0)) {
                    TickUsed(static_cast<EventID>(entityEvent.entity_uuid_),
                             entityEvent.pos_);
                }
                break;
            }
            default:
                g_logger.LogError("Dimension::EventTick", "Not handled yet!");
        }

        g_event_handler.ExecuteEvent(e, this);
    }

    EventQueue->clear();
    event_manager_.Swap();

    tick_usage_.clear();

    // Tick all entities

    world_->GetEntityContainer()->Tick(this);
}

void Dimension::Update() {
    std::vector<ChunkPos> requestedChunks =
        world_updater_->GetRequestedChunks();
    std::vector<ChunkPos> requestedLights =
        world_updater_->GetRequestedLightUpdates();

    chunk_generator_->Generate(requestedChunks);
    light_engine_->Generate(requestedLights);

    std::vector<std::unique_ptr<Chunk>> chunks = chunk_generator_->GetOutput();
    std::vector<std::unique_ptr<LightStorage>> lights =
        light_engine_->GetOutput();

    world_updater_->SetChunk(std::move(chunks));
    world_updater_->SetLight(std::move(lights));
}