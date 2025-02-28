#pragma once
#include <stdarg.h>
#include <variant>

#include "Event.h"
#include "../Chunk/Block/Block.h"
#include "BlockEventHandles/BlockHandles.h"
#include "EntityEventHandles/EntityHandles.h"


class _EventHandler {
private:
	using EventFunctionTypes = std::variant<
		void (*)(BlockID, const BlockPos&),
		void (*)(Event::EntityEvent)
	>;

	int event_count_ = 0;
	EventID RegisterBlockEvent(void (*func)(BlockID, const BlockPos&));

	EventID RegisterEntityEvent(void (*func)(Event::EntityEvent));

	FastHashMap<EventID, void (*)(BlockID, const BlockPos&)> BlockEventHandles;
	FastHashMap<EventID, void (*)(Event::EntityEvent)> EntityEventHandles;

public:
	EventID BlockPlace = RegisterBlockEvent(HandlePlaceBlock);
	EventID BlockTick = RegisterBlockEvent(HandleBlockTick);

	EventID SummonEntity = RegisterEntityEvent(HandleEntitySummon);
	EventID EntityTick = RegisterEntityEvent(HandleEntityTick);
	EventID RemoveEntity = RegisterEntityEvent(HandleRemoveEntity);

	void ExecuteEvent(Event::Event event);


} __declspec(selectany) EventHandler;