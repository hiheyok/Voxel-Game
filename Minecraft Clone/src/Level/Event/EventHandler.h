#pragma once
#include <unordered_map>

#include "Event.h"
#include "../Chunk/Block/Block.h"
#include "BlockEventHandles/BlockHandles.h"
#include "EntityEventHandles/EntityHandles.h"


class _EventHandler {
private:
	int counter = 0;
	EventID RegisterBlockEvent(void (*func)(BlockID, int, int, int));

	EventID RegisterEntityEvent(void (*func)(Event::EventDataType::_EntityEvent));

	std::unordered_map<EventID, void (*)(BlockID, int, int, int)> BlockEventHandles;
	std::unordered_map<EventID, void (*)(Event::EventDataType::_EntityEvent)> EntityEventHandles;

	

public:
	EventID BlockPlace = RegisterBlockEvent(HandlePlaceBlock);
	EventID DirtTick = RegisterBlockEvent(HandleDirtTick);
	EventID BlockTick = RegisterBlockEvent(HandleBlockTick);

	EventID SummonEntity = RegisterEntityEvent(HandleEntitySummon);
	EventID EntityTick = RegisterEntityEvent(HandleEntityTick);
	EventID RemoveEntity = RegisterEntityEvent(HandleRemoveEntity);

	void ExecuteEvent(Event event);


} __declspec(selectany) EventHandler;