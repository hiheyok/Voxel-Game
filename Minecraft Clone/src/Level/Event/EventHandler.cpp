#include "EventHandler.h"

void EventHandler::ExecuteEvent(Event::Event event, Dimension* dimension) {
    switch (event.type_) {
    case Event::BLOCK_EVENT:
    {
        const Event::BlockEvent& blockEvent = std::get<Event::BlockEvent>(event.event_data_);
        (*block_event_handles_[blockEvent.id_])(blockEvent, dimension);
        break;
    }
    case Event::ENTITY_EVENT:
    {
        const Event::EntityEvent& entityEvent = std::get<Event::EntityEvent>(event.event_data_);
        (*entity_event_handles_[entityEvent.id_])(entityEvent, dimension);
        break;
    }
    }
}

EventID EventHandler::RegisterBlockEvent(void (*func)(const Event::BlockEvent&, Dimension*)) {
    block_event_handles_[event_count_] = func;
    return event_count_++;
}
EventID EventHandler::RegisterEntityEvent(void (*func)(const Event::EntityEvent&, Dimension*)) {
    entity_event_handles_[event_count_] = func;
    return event_count_++;
}