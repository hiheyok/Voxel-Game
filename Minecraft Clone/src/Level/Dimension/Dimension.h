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
	World* world;
	DimensionProperties properties;
	WorldParameters worldSettings;
	
	FastHashMap<int, ska::flat_hash_set<ChunkPos>> TickUsage;

protected:
	WorldGeneratorID generatorType;

	void Start(DimensionProperties p) {
		world = new World;
		world->Initialize();
		properties = p;
		worldSettings.HorizontalTickingDistance = AppOptions.HorizontalRenderDistance;
		worldSettings.VerticalTickingDistance = AppOptions.VerticalRenderDistance;

		worldInteractions.init(world, worldSettings);


		if (Generators.GetGenerator(generatorType)->useTallChunks) {
			worldInteractions.UseTallGeneration();
		}
		
	}
public:
	WorldInteractions worldInteractions;
	EventSystem EventManager;

	Dimension(WorldGeneratorID generatorTypeIn = Generators.DEBUG) {
		generatorType = generatorTypeIn;
	}

	WorldGeneratorID getGeneratorType() {
		return generatorType;
	}
	
	// TODO: Use custom block classd

	bool CheckTickUsed(EventID id, const BlockPos& pos) {//temp sol
		return TickUsage[id].count(pos);
	}

	void TickUsed(EventID id, const BlockPos& pos) {//temp solution
		TickUsage[id].insert(pos);
	}

	void Initialize(DimensionProperties p) { //Generate new world
		Start(p);
	}

	virtual void Tick() { 
		
	}

	void EventTick() {
		std::vector<Event::Event>* EventQueue = EventManager.GetQueue();

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
		EventManager.Swap();

		TickUsage.clear();

		//Tick all entities

		world->Entities.Tick();
	}
};