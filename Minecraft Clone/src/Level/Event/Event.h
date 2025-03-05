#pragma once
#include <variant>

#include "../Entity/Entity.h"
#include "../Typenames.h"
#include "../Chunk/ChunkPos/ChunkPos.h"


constexpr size_t MAX_EVENT_SIZE = 32;


namespace Event {

    struct BlockEvent {
        BlockEvent(BlockPos pos, BlockID block, EventID id, char unique_id = 0) : pos_{ pos }, block_{ block }, id_{ id }, unique_id_{ unique_id } {}
        BlockEvent() {}

        BlockPos pos_; // 12 - 8
        BlockID block_; //1
        EventID id_ = 0; //8
        char unique_id_ = 0;
    };

    struct ChunkEvent {
        ChunkPos pos_;
        EventID id_ = 0;
        char unique_id_ = 0;
    };

    struct EntityEvent {
        BlockPos pos_{0,0,0};
        EntityTypeID entity_type_ = NULL;
        EntityUUID entity_uuid_ = NULL;
        EventID id_ = 0;
        char unique_id_ = 0;
    };

    enum EventType {
        NULL_EVENT,
        BLOCK_EVENT,
        ENTITY_EVENT,
        CHUNK_LOAD_EVENT,
        CHUNK_UNLOAD_EVENT
    };

    struct Event {
        EventType type_ = NULL_EVENT;
        int tick_time_ = 0;

        Event(const Event& other) : type_{ other.type_ },
            tick_time_{ other.tick_time_ },
            event_data_{ other.event_data_ } {}

        Event() {};

        template <typename EventType> 
        Event(const EventType& newEvent) {
            if constexpr (std::is_same_v<EventType, EntityEvent>) {
                type_ = ENTITY_EVENT;
            }
            else if constexpr (std::is_same_v<EventType, ChunkEvent>) {
                type_ = CHUNK_LOAD_EVENT;
            }
            else if constexpr (std::is_same_v<EventType, BlockEvent>) {
                type_ = BLOCK_EVENT;
            }
            else {
                throw std::runtime_error("Invalid event type!");
            }

            event_data_ = newEvent;
        }

        std::variant<BlockEvent, ChunkEvent, EntityEvent, std::monostate> event_data_;
    };

}
