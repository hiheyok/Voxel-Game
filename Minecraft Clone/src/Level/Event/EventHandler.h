#pragma once
#include <stdarg.h>
#include <variant>

#include "Event.h"
#include "../Chunk/Block/Block.h"
#include "BlockEventHandles/BlockHandles.h"
#include "EntityEventHandles/EntityHandles.h"

class EventHandler {
private:
    using EventFunctionTypes = std::variant<
        void (*)(BlockID, const BlockPos&, Dimension*),
        void (*)(Event::EntityEvent, Dimension*)
    >;

    int event_count_ = 0;
    EventID RegisterBlockEvent(void (*func)(const Event::BlockEvent&, Dimension*));

    EventID RegisterEntityEvent(void (*func)(const Event::EntityEvent&, Dimension*));

    FastHashMap<EventID, void (*)(const Event::BlockEvent&, Dimension*)> block_event_handles_;
    FastHashMap<EventID, void (*)(const Event::EntityEvent&, Dimension*)> entity_event_handles_;

public:

    // TODO: Naming scheme exception?
    EventID BlockPlace = RegisterBlockEvent(HandlePlaceBlock);
    EventID BlockTick = RegisterBlockEvent(HandleBlockTick);

    EventID SummonEntity = RegisterEntityEvent(HandleEntitySummon);
    EventID EntityTick = RegisterEntityEvent(HandleEntityTick);
    EventID RemoveEntity = RegisterEntityEvent(HandleRemoveEntity);

    void ExecuteEvent(Event::Event event, Dimension* dimension);


} inline g_event_handler;