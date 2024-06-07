#include "EventHandler.h"

void _EventHandler::ExecuteEvent(Event event) {
	Event::EventDataType data = event.Data;

	switch (event.Type) {
	case BLOCK_EVENT:
		(*BlockEventHandles[data.BlockEvent.id])(data.BlockEvent.block, data.BlockEvent.x, data.BlockEvent.y, data.BlockEvent.z);
		break;
	case ENTITY_EVENT:
		(*EntityEventHandles[data.EntityEvent.id])(data.EntityEvent);
		break;

	}
}

EventID _EventHandler::RegisterBlockEvent(void (*func)(BlockID, int, int, int)) {
	counter++;
	BlockEventHandles[counter] = func;
	return counter;
}
EventID _EventHandler::RegisterEntityEvent(void (*func)(Event::EventDataType::_EntityEvent)) {
	counter++;
	EntityEventHandles[counter] = func;
	return counter;
}