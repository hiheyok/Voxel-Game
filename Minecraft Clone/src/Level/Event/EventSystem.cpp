#include "EventSystem.h"

std::vector<Event::Event>* EventSystem::GetQueue() {
	return QueueActive;
}

void EventSystem::Swap() {
	std::vector<Event::Event>* tmp = QueueActive;
	QueueActive = QueueUnactive;
	QueueUnactive = tmp;
}