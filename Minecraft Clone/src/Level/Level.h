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
		levelLoader.Start(threadCount);
	} 

	void Stop() {
		levelLoader.Stop();
	}

	void updateDimensions() {
		std::vector<Chunk*> chunks = levelLoader.getGeneratedChunk();

		mainWorld.worldInteractions.addChunks(chunks);
	}

};