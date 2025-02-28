#pragma once
#include "Event.h"
#include <deque>


class EventSystem {
public:
	
	EventSystem() {
		NullEvent.type_ = Event::NULL_EVENT;

		QueueActive = new std::vector<Event::Event>;
		QueueUnactive = new std::vector<Event::Event>;

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
	std::vector<Event::Event>* GetQueue();

	template <class EventType> void AddEvent(EventType e) {
		QueueUnactive->emplace_back(e);
	}

	void Swap();

private:
	std::vector<Event::Event>* QueueActive;
	std::vector<Event::Event>* QueueUnactive;

	Event::Event NullEvent;
};