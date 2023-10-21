#pragma once

typedef unsigned long long int EventID;

enum EventType {
	BLOCK_EVENT, ENTITY_EVENT
};

struct Event
{
	EventID id;
};

struct BlockEvent : Event {
	long long x, y, z;
};