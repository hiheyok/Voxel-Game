#pragma once
#include "../World/World.h"
#include "DimensionProperties.h"
#include "../../World/TerrainGeneration/Generator.h"
#include "../World/WorldInteraction/WorldInteractions.h"
class Dimension {
private:
	World* world;
	DimensionProperties properties;
	WorldParameters worldSettings;
public:
	WorldInteractions worldInteractions;

	void Initialize(DimensionProperties p) { //Generate new world
		world = new World;
		properties = p;
		worldInteractions = WorldInteractions(world, worldSettings);
	}

	virtual void Tick() { 
		
	}
};