#include "EventHandler.h"
#include <intrin.h>

void _EventHandler::ExecuteEvent(Event event) {
	Event::EventDataType data = event.Data;

	switch (event.Type) {
	case BLOCK_EVENT:
		(*BlockEventHandles[data.BlockEvent.id])(data.BlockEvent.block, data.BlockEvent.pos);
		break;
	case ENTITY_EVENT:
		(*EntityEventHandles[data.EntityEvent.id])(data.EntityEvent);
		break;

	}
}

EventID _EventHandler::RegisterBlockEvent(void (*func)(BlockID, const BlockPos&)) {
	BlockEventHandles[m_eventCount] = func;
	return m_eventCount++;
}
EventID _EventHandler::RegisterEntityEvent(void (*func)(Event::EventDataType::_EntityEvent)) {
	EntityEventHandles[m_eventCount] = func;
	return m_eventCount++;
}