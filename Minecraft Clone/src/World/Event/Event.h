#pragma once

typedef unsigned long long int EventID;

constexpr size_t MAX_EVENT_SIZE = 16;

enum EventType {
	NULL_EVENT,
	BLOCK_EVENT,
	ENTITY_EVENT,
	CHUNK_LOAD_EVENT,
	CHUNK_UNLOAD_EVENT
};

union Event
{
	EventType Type = NULL_EVENT;

	struct _BlockEvent {
		int x = 0; int y = 0; int z = 0;
		EventID id = 0;

	} BlockEvent;

	struct _ChunkEvent {
		int x = 0; int y = 0; int z = 0;
		EventID id = 0;
	} ChunkEvent;

	struct _Data {
		char d[MAX_EVENT_SIZE]{};
	} Data;
};
