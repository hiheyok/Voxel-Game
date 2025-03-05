#include "EventSystem.h"

std::unique_ptr<std::vector<Event::Event>>& EventSystem::GetQueue() {
    return queue_active_;
}

void EventSystem::Swap() {
    std::unique_ptr<std::vector<Event::Event>> tmp = std::move(queue_active_);
    queue_active_ = std::move(queue_unactive_);
    queue_unactive_ = std::move(tmp);
}