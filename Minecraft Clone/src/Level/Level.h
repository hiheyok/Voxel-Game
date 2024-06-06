#pragma once
#include "Dimension/Dimension.h"
#include "LevelLoader.h"

class Level {
public:
	Dimension mainWorld;
	LevelLoader levelLoader;

	Level() {

	}

	void Start(int threadCount) {
		DimensionProperties properties;
		mainWorld.Initialize(properties);
		levelLoader.Start(threadCount, mainWorld.worldInteractions.worldLoader->getWorld());
		Block::DimensionPTR = &mainWorld;
	} 

	void Stop() {
		levelLoader.Stop();
	}

	void updateDimensions() {
		//Generated Chunks
		levelLoader.sendRequestedChunks(mainWorld.worldInteractions.worldLoader->getRequestedChunks());
		std::vector<Chunk*> chunks = levelLoader.getGeneratedChunk();
		mainWorld.worldInteractions.addChunks(chunks);

		//Updated Light Info
		levelLoader.sendRequestedLightUpdates(mainWorld.worldInteractions.getRequestedLightUpdates());
		std::vector<ChunkLightingContainer*> lighting = levelLoader.getLightingInfomation();
		mainWorld.worldInteractions.updateLighting(lighting);
	}

};