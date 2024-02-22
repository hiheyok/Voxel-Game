#pragma once

#include "Properties/EntityProperties.h"
#include "Type/EntityType.h"
#include "Entities.h"

typedef unsigned long long int EntityUUID; //UUID

struct Entity  {
	EntityTypeID Type = NULL; //State what type of entity is it. Zombie? Human??
	EntityProperty Properties; //Provide properties such as position, acceleration, etc.
	EntityUUID EntityUUID = 0x00000000;

	void Tick();
};