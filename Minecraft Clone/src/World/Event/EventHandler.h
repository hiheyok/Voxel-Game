#pragma once

#include "Event.h"
#include <unordered_map>

class _EventHandler {
public:
	EventID BlockPlace = 0x01;

	void ExecuteEvent() {

	}

private:
	
	std::unordered_map<EventID, void (*)(...)> BlockEventHandles;


} EventHandler;