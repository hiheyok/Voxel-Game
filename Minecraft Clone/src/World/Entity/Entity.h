#pragma once
#include "Properties/EntityProperties.h"
#include "../Typenames.h"


struct Entity  {
	EntityTypeID Type = NULL; //State what type of entity is it. Zombie? Human??
	EntityProperty Properties; //Provide properties such as position, acceleration, etc.
	EntityUUID EntityUUID = 0x00000000;

	bool isDirty = true;

	void Tick();
};