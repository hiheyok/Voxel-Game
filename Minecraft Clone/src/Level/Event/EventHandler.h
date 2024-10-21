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
		void (*)(BlockID, int, int, int),
		void (*)(Event::EventDataType::_EntityEvent)
	>;

	int counter = 0;
	EventID RegisterBlockEvent(void (*func)(BlockID, int, int, int));

	EventID RegisterEntityEvent(void (*func)(Event::EventDataType::_EntityEvent));

	FastHashMap<EventID, void (*)(BlockID, int, int, int)> BlockEventHandles;
	FastHashMap<EventID, void (*)(Event::EventDataType::_EntityEvent)> EntityEventHandles;


public:
	EventID BlockPlace = RegisterBlockEvent(HandlePlaceBlock);
	EventID DirtTick = RegisterBlockEvent(HandleDirtTick);
	EventID BlockTick = RegisterBlockEvent(HandleBlockTick);

	EventID SummonEntity = RegisterEntityEvent(HandleEntitySummon);
	EventID EntityTick = RegisterEntityEvent(HandleEntityTick);
	EventID RemoveEntity = RegisterEntityEvent(HandleRemoveEntity);

	void ExecuteEvent(Event event);


} __declspec(selectany) EventHandler;