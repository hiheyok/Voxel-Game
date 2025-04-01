#pragma once
#include "Level/Dimension/DimensionProperties.h"
#include "Level/DataContainer/EntityContainer.h"
#include "Level/Event/EventSystem.h"
#include "Level/Event/EventHandler.h"
#include "Level/TerrainGeneration/Generator.h"
#include "Level/TerrainGeneration/Generators/GeneratorType.h"
#include "Level/World/World.h"
#include "Level/World/WorldInteraction/WorldInteractions.h"
#include "Level/World/WorldParameters.h"
#include "Core/Options/Option.h"

class Dimension {
private:
    World* world_;
    DimensionProperties properties_;
    WorldParameters world_settings_;
    
    FastHashMap<int, FastHashSet<ChunkPos>> tick_usage_;

protected:
    WorldGeneratorID generator_type_;

    void Start(DimensionProperties properties) {
        world_ = new World;
        world_->Initialize();
        properties_ = properties;
        world_settings_.horizontal_ticking_distance_ = g_app_options.horizontal_render_distance_;
        world_settings_.vertical_ticking_distance_ = g_app_options.vertical_render_distance_;

        world_interactions_.init(world_, world_settings_);


        if (g_generators.GetGenerator(generator_type_)->use_tall_chunks_) {
            world_interactions_.UseTallGeneration();
        }
        
    }
public:
    WorldInteractions world_interactions_;
    EventSystem event_manager_;
    const int tick_rate_ = 20; //TODO: tmp fix

    Dimension(WorldGeneratorID generatorTypeIn = g_generators.DEBUG) {
        generator_type_ = generatorTypeIn;
        world_ = nullptr;
    }

    WorldGeneratorID GetGeneratorType() {
        return generator_type_;
    }

    bool CheckTickUsed(EventID id, const BlockPos& pos) {//temp sol
        return tick_usage_[id].count(pos);
    }

    void TickUsed(EventID id, const BlockPos& pos) {//temp solution
        tick_usage_[id].insert(pos);
    }

    void Initialize(DimensionProperties p) { //Generate new world
        Start(p);
    }

    virtual void Tick() { 
        
    }

    void EventTick() {
        std::unique_ptr<std::vector<Event>>& EventQueue = event_manager_.GetQueue();

        for (size_t i = 0; i < EventQueue->size(); i++) {
            const Event& e = EventQueue->at(i);
            switch (e.type_) {
            case NULL_EVENT:
                break;
            case BLOCK_EVENT:
            {
                const BlockEvent& blockEvent = std::get<BlockEvent>(e.event_data_);
                if (CheckTickUsed(blockEvent.id_, blockEvent.pos_)) {
                    continue;
                }
                TickUsed(blockEvent.id_, blockEvent.pos_);
                break;
            }
            case ENTITY_EVENT:
            {
                const EntityEvent& entityEvent = std::get<EntityEvent>(e.event_data_);
                if ((entityEvent.unique_id_ != 0) && CheckTickUsed(entityEvent.unique_id_, entityEvent.pos_)) {
                    continue;
                }
                // TODO: tmp fix rework this later
                if ((entityEvent.unique_id_ != 0)) {
                    TickUsed(static_cast<EventID>(entityEvent.entity_uuid_), entityEvent.pos_);
                }
                break;
            }
            default:
                throw std::exception("Not handled yet!");
            }

            g_event_handler.ExecuteEvent(e, this);
        }

        EventQueue->clear();
        event_manager_.Swap();

        tick_usage_.clear();

        //Tick all entities

        world_->entities_->Tick(this);
    }
};