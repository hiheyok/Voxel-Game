#pragma once
#include "Event.h"
#include <deque>


class EventSystem {
public:
	
	EventSystem() {
		NullEvent.Type = NULL_EVENT;

		QueueActive = new std::vector<Event>;
		QueueUnactive = new std::vector<Event>;

		QueueActive->reserve(16777215);
		QueueUnactive->reserve(16777215);
	}

	~EventSystem() {
		QueueActive->clear();
		QueueUnactive->clear();

		delete QueueActive;
		delete QueueUnactive;
	}

	//Gets event; if empty, return null event
	std::vector<Event>* GetQueue();

	void AddEvent(Event e);

	void Swap();

private:
	std::vector<Event>* QueueActive;
	std::vector<Event>* QueueUnactive;

	Event NullEvent;
};