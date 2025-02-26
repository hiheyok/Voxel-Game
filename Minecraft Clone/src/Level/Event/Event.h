#pragma once
#include "../Entity/Entity.h"
#include "../Typenames.h"
#include "../Chunk/ChunkPos/ChunkPos.h"


constexpr size_t MAX_EVENT_SIZE = 32;

enum EventType {
	NULL_EVENT,
	BLOCK_EVENT,
	ENTITY_EVENT,
	CHUNK_LOAD_EVENT,
	CHUNK_UNLOAD_EVENT
};

struct Event
{
	EventType Type = NULL_EVENT;

	int TickTime = 0;

	Event() {

	} 

	// TODO: Add chunk events

	Event(const Event& other)
		: Type(other.Type), TickTime(other.TickTime) // Copy primitive members
	{
		// Use placement new to copy the correct union member
		switch (other.Type) {
		case BLOCK_EVENT:
			new (&Data.BlockEvent) EventDataType::_BlockEvent(other.Data.BlockEvent);
			break;
		case ENTITY_EVENT:
			new (&Data.EntityEvent) EventDataType::_EntityEvent(other.Data.EntityEvent);
			break;
		default:
			new (&Data.__Data) EventDataType::_Data(other.Data.__Data);
			break;
		}
	}

	~Event() {
		// Manually destroy the union's active member
		switch (Type) {
		case BLOCK_EVENT:
			Data.BlockEvent.~_BlockEvent();
			break;
		case ENTITY_EVENT:
			Data.EntityEvent.~_EntityEvent();
			break;
		default:
			break;
		}
	}

	union EventDataType {
		EventDataType() {

		}

		EventDataType(const EventDataType& other) {
			std::memcpy(this, &other, sizeof(EventDataType));
		}

		// TODO: Use custom block pos class to store pos

		struct _BlockEvent { //32 Bytes
			BlockPos pos; // 12 - 8
			BlockID block; //1
			EventID id = 0; //8
			char UniqueID = 0;

		} BlockEvent;


		// TODO: Use custom entity pos class
		struct _EntityEvent { //32 byte
			BlockPos pos;
			EntityTypeID entityType;
			EntityUUID EntityID;
			EventID id = 0;
			char UniqueID = 0;
		} EntityEvent;

		struct _ChunkEvent { //24 byte
			ChunkPos pos;
			EventID id = 0;
			char UniqueID = 0;
		} ChunkEvent;

		struct _Data {
			char d[MAX_EVENT_SIZE]{};
		} __Data;
	} Data;
	
};
