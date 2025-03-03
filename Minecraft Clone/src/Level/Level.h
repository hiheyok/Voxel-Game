#pragma once
#include "Dimension/Dimension.h"
#include "LevelLoader.h"

class Level {
public:
	Dimension* main_world_  = nullptr;
	LevelLoader level_loader_;

	Level() {

	}

	void Start(int worldGenThreadCount, int light_engine_thread_count_);

	void Stop();

	void updateDimensions();

};