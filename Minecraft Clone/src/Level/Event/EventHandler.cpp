#include "Level/Event/EventHandler.h"

void EventHandler::ExecuteEvent(Event event, Dimension* dimension) {
    switch (event.type_) {
        case BLOCK_EVENT: {
            const BlockEvent& blockEvent =
                std::get<BlockEvent>(event.event_data_);
            (*block_event_handles_[blockEvent.id_])(blockEvent, dimension);
            break;
        }
        case ENTITY_EVENT: {
            const EntityEvent& entityEvent =
                std::get<EntityEvent>(event.event_data_);
            (*entity_event_handles_[entityEvent.id_])(entityEvent, dimension);
            break;
        }
    }
}

EventID EventHandler::RegisterBlockEvent(void (*func)(const BlockEvent&,
                                                      Dimension*)) {
    block_event_handles_[event_count_] = func;
    return event_count_++;
}
EventID EventHandler::RegisterEntityEvent(void (*func)(const EntityEvent&,
                                                       Dimension*)) {
    entity_event_handles_[event_count_] = func;
    return event_count_++;
}