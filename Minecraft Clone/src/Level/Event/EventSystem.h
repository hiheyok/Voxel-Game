#pragma once
#include "Event.h"
#include <deque>


class EventSystem {
public:
	
	EventSystem() {
		NullEvent.type_ = Event::NULL_EVENT;

		queue_active_ = new std::vector<Event::Event>;
		queue_unactive_ = new std::vector<Event::Event>;

		queue_active_->reserve(16777215);
		queue_unactive_->reserve(16777215);
	}

	~EventSystem() {
		queue_active_->clear();
		queue_unactive_->clear();

		delete queue_active_;
		delete queue_unactive_;
	}

	//Gets event; if empty, return null event
	std::vector<Event::Event>* GetQueue();

	template <class EventType> void AddEvent(EventType e) {
		queue_unactive_->emplace_back(e);
	}

	void Swap();

private:
	std::vector<Event::Event>* queue_active_;
	std::vector<Event::Event>* queue_unactive_;

	Event::Event NullEvent;
};