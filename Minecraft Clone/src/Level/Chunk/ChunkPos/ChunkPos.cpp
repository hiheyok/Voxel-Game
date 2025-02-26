#include "ChunkPos.h"

std::string std::to_string(const ChunkPos& obj) {
	return "[" + to_string(obj.x) + "," + to_string(obj.y) + "," + to_string(obj.z) + "]";
}