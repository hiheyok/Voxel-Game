#include "EventHandler.h"
#include <intrin.h>

void _EventHandler::ExecuteEvent(Event event) {
	Event::EventDataType data = event.Data;

	uint64_t tmp = 0;
	uint64_t diff = 0;
	switch (event.Type) {
	case BLOCK_EVENT:
	//	tmp = __rdtsc();
		(*BlockEventHandles[data.BlockEvent.id])(data.BlockEvent.block, data.BlockEvent.x, data.BlockEvent.y, data.BlockEvent.z);
		// diff = __rdtsc() - tmp;
	//	printf("%d\n", diff);
		break;
	case ENTITY_EVENT:
		(*EntityEventHandles[data.EntityEvent.id])(data.EntityEvent);
		break;

	}
}

EventID _EventHandler::RegisterBlockEvent(void (*func)(BlockID, int, int, int)) {
	BlockEventHandles[m_eventCount] = func;
	return m_eventCount++;
}
EventID _EventHandler::RegisterEntityEvent(void (*func)(Event::EventDataType::_EntityEvent)) {
	EntityEventHandles[m_eventCount] = func;
	return m_eventCount++;
}