#pragma once
#include "Event.h"
#include <deque>


class EventSystem {
public:
    
    EventSystem() {
        NullEvent.type_ = Event::NULL_EVENT;

        queue_active_ = std::make_unique<std::vector<Event::Event>>();
        queue_unactive_ = std::make_unique<std::vector<Event::Event>>();

        queue_active_->reserve(UINT16_MAX);
        queue_unactive_->reserve(UINT16_MAX);
    }

    ~EventSystem() {
        queue_active_->clear();
        queue_unactive_->clear();
    }

    //Gets event; if empty, return null event
    std::unique_ptr<std::vector<Event::Event>>& GetQueue();

    template <class EventType> void AddEvent(EventType e) {
        queue_unactive_->emplace_back(e);
    }

    void Swap();

private:
    std::unique_ptr<std::vector<Event::Event>> queue_active_;
    std::unique_ptr<std::vector<Event::Event>> queue_unactive_;

    Event::Event NullEvent;
};