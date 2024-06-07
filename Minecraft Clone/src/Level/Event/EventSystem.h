#pragma once
#include "Event.h"
#include <deque>
#include <unordered_map>


class EventSystem {
public:
	
	EventSystem() {
		NullEvent.Type = NULL_EVENT;
	}
	//Gets event; if empty, return null event
	Event GetNextEvent();

	void AddEvent(Event e);

	size_t getSize();

private:
	std::deque<Event> EventQueue;

	Event NullEvent;
};