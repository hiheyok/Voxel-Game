#pragma once
#ifndef EVENT_HANDLES
#define EVENT_HANDLES
#include <unordered_map>

#include "Event.h"
#include "../../World/Chunk/Block/Block.h"
#include "BlockEventHandles/BlockHandles.h"


class _EventHandler {
private:
	int counter = 0;
	EventID RegisterBlockEvent(void (*func)(BlockID, int, int, int)) {
		counter++;
		BlockEventHandles[counter] = func;
		return counter;
	}

	std::unordered_map<EventID, void (*)(BlockID, int, int, int)> BlockEventHandles;

	

public:
	EventID BlockPlace = RegisterBlockEvent(HandlePlaceBlock);
	EventID DirtTick = RegisterBlockEvent(HandleDirtTick);
	EventID BlockTick = RegisterBlockEvent(HandleBlockTick);

	void ExecuteEvent(Event event) {
		if (event.Type = BLOCK_EVENT) {
			Event::EventDataType data = event.Data;
			(*BlockEventHandles[data.BlockEvent.id])(data.BlockEvent.block, data.BlockEvent.x, data.BlockEvent.y, data.BlockEvent.z);
		}
	}


} __declspec(selectany) EventHandler;

#endif // !EVENT_HANDLES