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

		std::vector<Event>* EventQueue = EventManager.GetQueue();

		for (int i = 0; i < EventQueue->size(); i++) {
			Event e = EventQueue->at(i);
			switch (e.Type) {
			case NULL_EVENT:
				break;
			case BLOCK_EVENT:
				if (CheckTickUsed(e.Data.BlockEvent.id, e.Data.BlockEvent.pos)) {
					continue;
				}
				TickUsed(e.Data.BlockEvent.id, e.Data.BlockEvent.pos);
				break;
			case ENTITY_EVENT:
				if ((e.Data.EntityEvent.UniqueID != 0) && CheckTickUsed(e.Data.EntityEvent.UniqueID, e.Data.EntityEvent.pos)) {
					continue;
				}
				if ((e.Data.EntityEvent.UniqueID != 0)) {
					TickUsed(e.Data.BlockEvent.id, e.Data.BlockEvent.pos);
				}
				break;
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