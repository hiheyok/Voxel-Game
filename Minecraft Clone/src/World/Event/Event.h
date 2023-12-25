#pragma once

typedef unsigned long long int EventID;

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

	union EventDataType {
		EventDataType() {

		}

		struct _BlockEvent { //21 Bytes
			int z = 0; int y = 0; int x = 0; // 12
			BlockID block; //1
			EventID id = 0; //8

		} BlockEvent;

		struct _ChunkEvent {
			int x = 0; int y = 0; int z = 0;
			EventID id = 0;
		} ChunkEvent;

		struct _Data {
			char d[MAX_EVENT_SIZE]{};
		} __Data;
	} Data;
	
};
