#pragma once
#include "../Type/EntityType.h"

class Player : public Entity {
public:
	EntityInventory m_EntityInventory;

	Player() {
		Properties.isChunkLoader = true;
	}

private:
};