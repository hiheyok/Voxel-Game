#pragma once
#include "Properties/EntityProperties.h"
#include "../Typenames.h"
#include "../Item/Inventory/EntityInventory.h"

extern int AmountEntityDestroy;

struct Entity  {
	
	EntityProperty Properties; //Provide properties such as position, acceleration, etc.

	bool isDirty = true;
	int dimensionID = NULL; //provide dimension location

	void Tick();
};
