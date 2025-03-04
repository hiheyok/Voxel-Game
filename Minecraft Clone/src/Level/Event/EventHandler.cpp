#include "EventHandler.h"

void EventHandler::ExecuteEvent(Event::Event event) {
	switch (event.type_) {
	case Event::BLOCK_EVENT:
	{
		const Event::BlockEvent& blockEvent = std::get<Event::BlockEvent>(event.event_data_);
		(*block_event_handles_[blockEvent.id_])(blockEvent.block_, blockEvent.pos_);
		break;
	}
	case Event::ENTITY_EVENT:
	{
		const Event::EntityEvent& entityEvent = std::get<Event::EntityEvent>(event.event_data_);
		(*entity_event_handles_[entityEvent.id_])(entityEvent);
		break;
	}
	}
}

EventID EventHandler::RegisterBlockEvent(void (*func)(BlockID, const BlockPos&)) {
	block_event_handles_[event_count_] = func;
	return event_count_++;
}
EventID EventHandler::RegisterEntityEvent(void (*func)(Event::EntityEvent)) {
	entity_event_handles_[event_count_] = func;
	return event_count_++;
}