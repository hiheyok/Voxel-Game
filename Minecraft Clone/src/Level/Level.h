#pragma once
#include "Dimension/Dimension.h"
#include "LevelLoader.h"

class Level {
public:
	Dimension* mainWorld;
	LevelLoader levelLoader;

	Level() {

	}

	void Start(int worldGenThreadCount, int lightEngineThreadCount);

	void Stop();

	void updateDimensions();

};