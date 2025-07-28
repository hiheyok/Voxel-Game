// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Event/EventSystem.h"

EventSystem::EventSystem() {
  NullEvent.type_ = NULL_EVENT;

  queue_active_ = std::make_unique<std::vector<Event>>();
  queue_unactive_ = std::make_unique<std::vector<Event>>();

  queue_active_->reserve(UINT16_MAX);
  queue_unactive_->reserve(UINT16_MAX);
}

EventSystem::~EventSystem() {
  queue_active_->clear();
  queue_unactive_->clear();
}

std::vector<Event>& EventSystem::GetQueue() {
  return *queue_active_.get();
}

void EventSystem::Swap() {
  std::unique_ptr<std::vector<Event>> tmp = std::move(queue_active_);
  queue_active_ = std::move(queue_unactive_);
  queue_unactive_ = std::move(tmp);
}
