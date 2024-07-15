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
	
	ska::flat_hash_map<int, ska::flat_hash_set<ChunkID>> TickUsage;

protected:
	WorldGeneratorID generatorType;

	void Start(DimensionProperties p) {
		world = new World;
		world->Initialize();
		properties = p;
		worldInteractions.init(world, WorldParameters());
		worldSettings.HorizontalTickingDistance = AppOptions.HorizontalRenderDistance;
		worldSettings.VerticalTickingDistance = AppOptions.VerticalRenderDistance;

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
	

	bool CheckTickUsed(EventID id, int x, int y, int z) {//temp sol
		return TickUsage[id].count(getChunkID(x, y, z));
	}

	void TickUsed(EventID id, int x, int y, int z) {//temp solution
		TickUsage[id].insert(getChunkID(x, y, z));
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
				if (CheckTickUsed(e.Data.BlockEvent.id, e.Data.BlockEvent.x, e.Data.BlockEvent.y, e.Data.BlockEvent.z)) {
					continue;
				}
				TickUsed(e.Data.BlockEvent.id, e.Data.BlockEvent.x, e.Data.BlockEvent.y, e.Data.BlockEvent.z);
				break;
			case ENTITY_EVENT:
				if ((e.Data.EntityEvent.UniqueID != 0) && CheckTickUsed(e.Data.EntityEvent.UniqueID, e.Data.EntityEvent.x, e.Data.EntityEvent.y, e.Data.EntityEvent.z)) {
					continue;
				}
				if ((e.Data.EntityEvent.UniqueID != 0)) {
					TickUsed(e.Data.BlockEvent.id, e.Data.BlockEvent.x, e.Data.BlockEvent.y, e.Data.BlockEvent.z);
				}
				break;
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