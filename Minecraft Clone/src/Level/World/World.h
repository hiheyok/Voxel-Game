#pragma once
#include "WorldDataAccess.h"
#include "Collusion/WorldCollusion.h"

class World : public WorldAccess {
private:
	WorldCollusionDetector Collusions;
public:
	void Initialize() {
		Collusions.Initialize(this);
	}

};