#include "EventSystem.h"

std::vector<Event>* EventSystem::GetQueue() {
	return QueueActive;
}

void EventSystem::AddEvent(Event e) {
	QueueUnactive->emplace_back(e);
}


void EventSystem::Swap() {
	std::vector<Event>* tmp = QueueActive;
	QueueActive = QueueUnactive;
	QueueUnactive = tmp;
}