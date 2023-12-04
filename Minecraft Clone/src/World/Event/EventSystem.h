#pragma once
#include "Event.h"
#include <deque>
#include <unordered_map>


class EventSystem {
public:
	
	EventSystem() {
		NullEvent.Type = NULL_EVENT;
	}

	Event GetNextEvent() {
		if (EventQueue.empty()) {
			return NullEvent;
		}

		Event e = EventQueue.front();
		EventQueue.pop_front();
		return e;
	}

	void AddEvent(Event e) {
		EventQueue.push_back(e);
	}

private:
	std::deque<Event> EventQueue;

	Event NullEvent;
};