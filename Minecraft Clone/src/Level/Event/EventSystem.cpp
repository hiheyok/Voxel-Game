#include "EventSystem.h"

std::vector<Event::Event>* EventSystem::GetQueue() {
	return queue_active_;
}

void EventSystem::Swap() {
	std::vector<Event::Event>* tmp = queue_active_;
	queue_active_ = queue_unactive_;
	queue_unactive_ = tmp;
}