#include "WorldLoader.h"

using namespace glm;
using namespace std;

void WorldLoader::loadSummonEntitySurrounding(EntityUUID uuid) {
	Entity* e = world->Entities.getEntity(uuid);

	if (e == nullptr) throw exception(std::string("Entity with UUID " + to_string(uuid) + " not found").c_str());

	vec3 pos = e->Properties.Position / 16.f;
	vec3 velocity = e->Properties.Velocity;

	ivec3 InitalPos((int)pos.x, (int)pos.y, (int)pos.z);

	deque<ivec3> FIFO;

	FIFO.push_back(InitalPos);

	while (!FIFO.empty()) {

		ivec3 ChunkPos = FIFO.front();
		FIFO.pop_front();

		//Gets relative position from the entity
		ivec3 Offset = InitalPos - ChunkPos;

		//Checks if it is in range
		if ((abs(Offset.x) > settings.HorizontalTickingDistance) || (abs(Offset.z) > settings.HorizontalTickingDistance))
			continue;
		if (abs(Offset.y) > settings.VerticalTickingDistance)
			continue;

		//If it is met check if it already exist of in process of generating
		if (world->getChunk(ChunkPos.x, ChunkPos.y, ChunkPos.z) != nullptr)
			continue;

		if (GeneratingChunk.count(getChunkID(ChunkPos)))
			continue;

		//Request chunk
		GeneratingChunk.insert(getChunkID(ChunkPos));
		ChunkRequest.emplace_back(getChunkID(ChunkPos));

		for (int side = 0; side < 3; side++) {

			ivec3 newObj = ChunkPos;

			if (velocity[side] == 0) {
				continue;
			}
			else if (velocity[side] > 0) {
				newObj[side] += 1;
			}
			else {
				newObj[side] += -1;
			}

			FIFO.push_back(newObj);
		}
	}

}

//Only work on loading chunks for not. Unloading for later
void WorldLoader::loadSurroundedMovedEntityChunk() {
	vector<ivec3> centerPositionList = {};
	vector<vec3> centerVelocityList = {};

	//Get entity chunk position

	for (const auto& e : EntityChunkLoaders) {
		Entity* entity = world->Entities.getEntity(e);

		if (entity == nullptr) throw exception(string("Entity with UUID " + to_string(e) + " not found").c_str());

		int x = static_cast<int>(entity->Properties.Position.x / 16.f);
		int y = static_cast<int>(entity->Properties.Position.y / 16.f);
		int z = static_cast<int>(entity->Properties.Position.z / 16.f);

		centerPositionList.push_back(ivec3(x, y, z));
		centerPositionList.push_back(entity->Properties.Velocity);
	}

	ivec3 AxisTickingDistance(settings.HorizontalTickingDistance, settings.VerticalTickingDistance, settings.HorizontalTickingDistance);

	for (int i = 0; i < centerPositionList.size(); i++) {
		ivec3 pos = centerPositionList[i];
		vec3 vel = centerVelocityList[i];

		for (int j = 0; j < 3; j++) {
			int side = 0;

			if (vel[j] > 0) {
				side = 1;
			}

			if (vel[j] < 0) {
				side = -1;
			}

			if (side == 0) continue;

			int uDistance = AxisTickingDistance[(j + 1) % 3];
			int vDistance = AxisTickingDistance[(j + 2) % 3];
			int SideDistanceOffset = AxisTickingDistance[j] + 1;

			for (int u = -uDistance; u <= uDistance; u++) {
				for (int v = -vDistance; v <= uDistance; v++) {
					ivec3 testPosition = pos;

					pos[(j + 1) % 3] += u;
					pos[(j + 2) % 3] += v;
					pos[j] += SideDistanceOffset * side;

					//Test if it exist of generating
					if (world->getChunk(testPosition)) continue;
					if (GeneratingChunk.count(getChunkID(testPosition))) continue;

					GeneratingChunk.insert(getChunkID(testPosition));
					ChunkRequest.push_back(getChunkID(testPosition));

				}
			}




		}
	}
}

void WorldLoader::loadSpawnChunks() {
	if (world == nullptr) throw exception("World is not initialized. Couldn't set spawn chunks");

	for (int x = -settings.SpawnChunkHorizontalRadius; x <= settings.SpawnChunkHorizontalRadius; x++) {
		for (int z = -settings.SpawnChunkHorizontalRadius; z <= settings.SpawnChunkHorizontalRadius; z++) {
			for (int y = -settings.SpawnChunkVerticalRadius; y <= settings.SpawnChunkVerticalRadius; y++) {
				if (world->getChunk(x, y, z) != nullptr) continue;

				ChunkID id = getChunkID(x, y, z);

				if (GeneratingChunk.count(id)) continue;

				GeneratingChunk.insert(id);
				ChunkRequest.emplace_back(id);

			}
		}
	}

	isSpawnChunksLoaded = true;
}