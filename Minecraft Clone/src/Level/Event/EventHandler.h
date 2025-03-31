#pragma once
#include <stdarg.h>
#include <variant>

#include "Event.h"
#include "../Chunk/Block/Block.h"
#include "BlockEventHandles/BlockHandles.h"
#include "EntityEventHandles/EntityHandles.h"

class EventHandler {
public:
    FastHashMap<EventID, void (*)(const BlockEvent&, Dimension*)> block_event_handles_;
    FastHashMap<EventID, void (*)(const EntityEvent&, Dimension*)> entity_event_handles_;

    EventID BlockPlace = RegisterBlockEvent(HandlePlaceBlock);
    EventID BlockTick = RegisterBlockEvent(HandleBlockTick);

    EventID SummonEntity = RegisterEntityEvent(HandleEntitySummon);
    EventID EntityTick = RegisterEntityEvent(HandleEntityTick);
    EventID RemoveEntity = RegisterEntityEvent(HandleRemoveEntity);

    void ExecuteEvent(Event event, Dimension* dimension);
private:
    using EventFunctionTypes = std::variant<
        void (*)(BlockID, const BlockPos&, Dimension*),
        void (*)(EntityEvent, Dimension*)
    >;

    int event_count_ = 0;
    EventID RegisterBlockEvent(void (*func)(const BlockEvent&, Dimension*));
    EventID RegisterEntityEvent(void (*func)(const EntityEvent&, Dimension*));

    
} inline g_event_handler;