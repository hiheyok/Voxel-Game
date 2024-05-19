#pragma once
#ifndef EVENT_HANDLES
#define EVENT_HANDLES
#include <unordered_map>

#include "Event.h"
#include "../../World/Chunk/Block/Block.h"
#include "BlockEventHandles/BlockHandles.h"
#include "EntityEventHandles/EntityHandles.h"


class _EventHandler {
private:
	int counter = 0;
	EventID RegisterBlockEvent(void (*func)(BlockID, int, int, int)) {
		counter++;
		BlockEventHandles[counter] = func;
		return counter;
	}
	EventID RegisterEntityEvent(void (*func)(Event::EventDataType::_EntityEvent)) {
		counter++;
		EntityEventHandles[counter] = func;
		return counter;
	}


	std::unordered_map<EventID, void (*)(BlockID, int, int, int)> BlockEventHandles;
	std::unordered_map<EventID, void (*)(Event::EventDataType::_EntityEvent)> EntityEventHandles;

	

public:
	EventID BlockPlace = RegisterBlockEvent(HandlePlaceBlock);
	EventID DirtTick = RegisterBlockEvent(HandleDirtTick);
	EventID BlockTick = RegisterBlockEvent(HandleBlockTick);

	EventID SummonEntity = RegisterEntityEvent(HandleEntitySummon);
	EventID EntityTick = RegisterEntityEvent(HandleEntityTick);
	EventID RemoveEntity = RegisterEntityEvent(HandleRemoveEntity);

	void ExecuteEvent(Event event) {
		Event::EventDataType data = event.Data;

		switch (event.Type) {
		case BLOCK_EVENT:
			(*BlockEventHandles[data.BlockEvent.id])(data.BlockEvent.block, data.BlockEvent.x, data.BlockEvent.y, data.BlockEvent.z);
			break;
		case ENTITY_EVENT:
			(*EntityEventHandles[data.EntityEvent.id])(data.EntityEvent);
			break;
		default:

		}
	}


} __declspec(selectany) EventHandler;

#endif // !EVENT_HANDLES