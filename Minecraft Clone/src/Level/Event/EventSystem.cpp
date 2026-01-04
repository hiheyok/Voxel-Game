// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Event/EventSystem.h"

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "Level/Event/Event.h"

using std::make_unique;
using std::move;
using std::unique_ptr;
using std::vector;

EventSystem::EventSystem() {
  NullEvent.type_ = NULL_EVENT;

  queue_active_ = make_unique<vector<Event>>();
  queue_unactive_ = make_unique<vector<Event>>();

  queue_active_->reserve(UINT16_MAX);
  queue_unactive_->reserve(UINT16_MAX);
}

EventSystem::~EventSystem() {
  queue_active_->clear();
  queue_unactive_->clear();
}

vector<Event>& EventSystem::GetQueue() { return *queue_active_.get(); }

void EventSystem::Swap() {
  unique_ptr<vector<Event>> tmp = move(queue_active_);
  queue_active_ = move(queue_unactive_);
  queue_unactive_ = move(tmp);
}
