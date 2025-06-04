// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Event/EventHandler.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

EventHandler::EventHandler(GameContext& game_context)
    : game_context_{game_context} {}
EventHandler::~EventHandler() = default;

void EventHandler::ExecuteEvent(Event event, Dimension* dimension) {
  switch (event.type_) {
    case BLOCK_EVENT: {
      const BlockEvent& blockEvent = std::get<BlockEvent>(event.event_data_);
      (*block_event_handles_[blockEvent.id_])(blockEvent, dimension);
      break;
    }
    case ENTITY_EVENT: {
      const EntityEvent& entityEvent = std::get<EntityEvent>(event.event_data_);
      (*entity_event_handles_[entityEvent.id_])(entityEvent, dimension);
      break;
    }
    default: {
      game_context_.logger_->LogWarn("EventHandler::ExecuteEvent",
                                     "Tried to handle unimplemented case");
      break;
    }
  }
}

EventID EventHandler::RegisterBlockEvent(void (*func)(const BlockEvent&,
                                                      Dimension*)) {
  block_event_handles_[event_count_] = func;
  return event_count_++;
}
EventID EventHandler::RegisterEntityEvent(void (*func)(const EntityEvent&,
                                                       Dimension*)) {
  entity_event_handles_[event_count_] = func;
  return event_count_++;
}
