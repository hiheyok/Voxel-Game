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
	Event GetNextEvent() {
		Event e = EventQueue.front();
		EventQueue.pop_front();
		return e;
	}

	void AddEvent(Event e) {
		EventQueue.push_back(e);
	}

	size_t getSize() {
		return EventQueue.size();
	}

private:
	std::deque<Event> EventQueue;

	Event NullEvent;
};