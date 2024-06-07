#include "EventSystem.h"

Event EventSystem::GetNextEvent() {
	Event e = EventQueue.front();
	EventQueue.pop_front();

	return e;
}

void EventSystem::AddEvent(Event e) {
	EventQueue.push_back(e);

}

size_t EventSystem::getSize() {
	return EventQueue.size();
}