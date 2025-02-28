#include "EventHandler.h"

void _EventHandler::ExecuteEvent(Event::Event event) {
	switch (event.type_) {
	case Event::BLOCK_EVENT:
	{
		const Event::BlockEvent& blockEvent = std::get<Event::BlockEvent>(event.event_data_);
		(*BlockEventHandles[blockEvent.id_])(blockEvent.block_, blockEvent.pos_);
		break;
	}
	case Event::ENTITY_EVENT:
	{
		const Event::EntityEvent& entityEvent = std::get<Event::EntityEvent>(event.event_data_);
		(*EntityEventHandles[entityEvent.id_])(entityEvent);
		break;
	}
	}
}

EventID _EventHandler::RegisterBlockEvent(void (*func)(BlockID, const BlockPos&)) {
	BlockEventHandles[event_count_] = func;
	return event_count_++;
}
EventID _EventHandler::RegisterEntityEvent(void (*func)(Event::EntityEvent)) {
	EntityEventHandles[event_count_] = func;
	return event_count_++;
}