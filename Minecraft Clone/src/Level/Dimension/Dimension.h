#pragma once
#include "../World/World.h"
#include "DimensionProperties.h"
#include "../TerrainGeneration/Generator.h"
#include "../World/WorldInteraction/WorldInteractions.h"
#include "../Event/EventSystem.h"
#include "../Event/EventHandler.h"
#include "../../Core/Options/Option.h"
#include "../TerrainGeneration/Generators/GeneratorType.h"
#include "../../Utils/Containers/skaHashmap.h"

class Dimension {
private:
	World* world_;
	DimensionProperties properties_;
	WorldParameters world_settings_;
	
	FastHashMap<int, ska::flat_hash_set<ChunkPos>> tick_usage_;

protected:
	WorldGeneratorID generator_type_;

	void Start(DimensionProperties properties) {
		world_ = new World;
		world_->Initialize();
		properties_ = properties;
		world_settings_.horizontal_ticking_distance_ = AppOptions.horizontal_render_distance_;
		world_settings_.vertical_ticking_distance_ = AppOptions.vertical_render_distance_;

		world_interactions_.init(world_, world_settings_);


		if (g_generators.GetGenerator(generator_type_)->use_tall_chunks_) {
			world_interactions_.UseTallGeneration();
		}
		
	}
public:
	WorldInteractions world_interactions_;
	EventSystem event_manager_;

	Dimension(WorldGeneratorID generatorTypeIn = g_generators.DEBUG) {
		generator_type_ = generatorTypeIn;
	}

	WorldGeneratorID GetGeneratorType() {
		return generator_type_;
	}
	
	// TODO: Use custom block classd

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
		std::vector<Event::Event>* EventQueue = event_manager_.GetQueue();

		for (int i = 0; i < EventQueue->size(); i++) {
			const Event::Event& e = EventQueue->at(i);
			switch (e.type_) {
			case Event::NULL_EVENT:
				break;
			case Event::BLOCK_EVENT:
			{
				const Event::BlockEvent& blockEvent = std::get<Event::BlockEvent>(e.event_data_);
				if (CheckTickUsed(blockEvent.id_, blockEvent.pos_)) {
					continue;
				}
				TickUsed(blockEvent.id_, blockEvent.pos_);
				break;
			}
			case Event::ENTITY_EVENT:
			{
				const Event::EntityEvent& entityEvent = std::get<Event::EntityEvent>(e.event_data_);
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

			EventHandler.ExecuteEvent(e);
		}

		EventQueue->clear();
		event_manager_.Swap();

		tick_usage_.clear();

		//Tick all entities

		world_->entities_.Tick();
	}
};