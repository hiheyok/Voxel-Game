#pragma once
#include <vector>

#include "Level/Event/Event.h"

class EventSystem {
   public:
    EventSystem();
    ~EventSystem();

    // Gets event; if empty, return null event
    std::unique_ptr<std::vector<Event>>& GetQueue();

    template <class EventType>
    void AddEvent(EventType e) {
        queue_unactive_->emplace_back(e);
    }

    void Swap();

   private:
    std::unique_ptr<std::vector<Event>> queue_active_;
    std::unique_ptr<std::vector<Event>> queue_unactive_;

    Event NullEvent;
};