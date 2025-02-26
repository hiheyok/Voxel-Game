#include "WorldLoader.h"

using namespace glm;
using namespace std;

void WorldLoader::loadSummonEntitySurrounding(EntityUUID uuid) {
	Entity* e = world->Entities.getEntity(uuid);

	if (e == nullptr) throw exception(std::string("Entity with UUID " + to_string(uuid) + " not found").c_str());

	vec3 pos = e->Properties.Position / 16.f;
	// vec3 velocity = e->Properties.Velocity;

	ivec3 InitalPos((int)pos.x, (int)pos.y, (int)pos.z);

	// TODO: Use light engine FIFO 
	deque<ivec3> FIFO;

	FIFO.push_back(InitalPos);
	while (!FIFO.empty()) {

		ivec3 chunkPos = FIFO.front();
		FIFO.pop_front();

		//Gets relative position from the entity
		ivec3 Offset = InitalPos - chunkPos;

		//Checks if it is in range
		if ((abs(Offset.x) > settings.HorizontalTickingDistance) || (abs(Offset.z) > settings.HorizontalTickingDistance))
			continue;
		if (abs(Offset.y) > settings.VerticalTickingDistance)
			continue;

		bool isSuccess = RequestLoad(ChunkPos{ chunkPos.x, chunkPos.y, chunkPos.z });
		if (!isSuccess) continue;

		for (int side = 0; side < 6; side++) {

			ivec3 newObj = chunkPos;

			newObj[side >> 1] += (side & 0b1) * 2 - 1;

			FIFO.push_back(newObj);
		}
	}

}

bool WorldLoader::RequestLoad(const ChunkPos& pos) {
	ChunkPos p = pos;
	if (world->getChunk(p) != nullptr)
		return false;

	if (tallGeneration)
		p.y /= 16;

	if (GeneratingChunk.count(p))
		return false;

	//Request chunk
	GeneratingChunk.insert(p);
	ChunkRequest.emplace_back(p);
	return true;
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

		if ((entity->Properties.Velocity.x == 0.f) && (entity->Properties.Velocity.y == 0.f) && (entity->Properties.Velocity.z == 0.f)) {
			continue;
		}
		
		centerPositionList.push_back(ivec3(x, y, z));
		centerVelocityList.push_back(entity->Properties.Velocity);
	}

	if (centerPositionList.empty()) return;

	ivec3 AxisTickingDistance(settings.HorizontalTickingDistance, settings.VerticalTickingDistance, settings.HorizontalTickingDistance);

	int ChunkPadding = 4;

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

			int uDistance = AxisTickingDistance[(j + 1) % 3] + ChunkPadding;
			int vDistance = AxisTickingDistance[(j + 2) % 3] + ChunkPadding;
			int SideDistanceOffset = AxisTickingDistance[j] + 1;

			for (int u = -uDistance; u <= uDistance; u++) {
				for (int v = -vDistance; v <= uDistance; v++) {
					ivec3 testPosition = pos;

					testPosition[(j + 1) % 3] += u;
					testPosition[(j + 2) % 3] += v;
					testPosition[j] += SideDistanceOffset * side;

					//Test if it exist of generating
					bool isSuccess = RequestLoad(ChunkPos{ testPosition.x, testPosition.y, testPosition.z });
					if (!isSuccess) continue;	
				}
			}




		}
	}
}

void WorldLoader::loadSpawnChunks() {
	if (world == nullptr) throw exception("World is not initialized. Couldn't set spawn chunks");
	return;
	for (int x = -settings.SpawnChunkHorizontalRadius; x <= settings.SpawnChunkHorizontalRadius; x++) {
		for (int z = -settings.SpawnChunkHorizontalRadius; z <= settings.SpawnChunkHorizontalRadius; z++) {
			for (int y = -settings.SpawnChunkVerticalRadius; y <= settings.SpawnChunkVerticalRadius; y++) {
				bool isSuccess = RequestLoad(ChunkPos{ x, y, z });

				if (!isSuccess) {
					continue;
				}

			}
		}
	}

	isSpawnChunksLoaded = true;
}

WorldAccess* WorldLoader::getWorld() {
	return static_cast<WorldAccess*>(world);
}

//TODO: Fix me
void WorldLoader::replaceLightInfomation(ChunkLightingContainer* lighting) {
	ChunkColumn* col = world->getColumn(lighting->position_);
	int y = lighting->position_.y & 0b11111;
	if (col == nullptr) return; //fix this nullptr

	col->replaceLightContainer(y, lighting);
}

vector<ChunkPos> WorldLoader::getRequestedChunks() {
	vector<ChunkPos> tmp;
	lock.lock();
	tmp = ChunkRequest;
	ChunkRequest.clear();
	lock.unlock();

	return tmp;
}



void WorldLoader::addEntityChunkLoader(EntityUUID uuid) {
	EntityChunkLoaders.insert(uuid);
	loadSummonEntitySurrounding(uuid);
}

void WorldLoader::deleteEntityChunkLoader(EntityUUID uuid) {
	if (!EntityChunkLoaders.count(uuid))
		throw exception(string("Could not find entity with UUID " + to_string(uuid)).c_str());

	EntityChunkLoaders.erase(uuid);
}

bool WorldLoader::checkEntityExistChunkLoader(EntityUUID uuid) {
	return EntityChunkLoaders.count(uuid);
}

void WorldLoader::load() {
	if (!isSpawnChunksLoaded) loadSpawnChunks();

	loadSurroundedMovedEntityChunk();
	unloadSurroundedMovedEntityChunk();
}

void WorldLoader::addChunk(Chunk* chunk) {
	ChunkPos pos = chunk->position_;

	if (GeneratingChunk.count(pos)) GeneratingChunk.erase(pos);

	world->setChunk(chunk);
}