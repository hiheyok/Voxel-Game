#include "WorldInteractions.h"

using namespace std;
using namespace glm;

void WorldInteractions::init(World* w, WorldParameters parameters) {
	world = w;
	settings = parameters;
	worldLoader = new WorldLoader(w, parameters);
	Collusions.Initialize(static_cast<WorldAccess*>(w));
}

void WorldInteractions::summonEntity(Entity& entity) {
	if (world == nullptr) throw exception("Cannot summon entity. World is null");
	world->Entities.AddEntities(entity);

	if (entity.Properties.isChunkLoader)
		worldLoader->addEntityChunkLoader(entity.Properties.EntityUUID);
}

vector<ChunkID> WorldInteractions::getUpdatedChunkIDs() {
	vector<ChunkID> chunkIDs = {};
	UpdatedChunkLock.lock();
	chunkIDs.insert(chunkIDs.end(), UpdatedChunk.begin(), UpdatedChunk.end());
	UpdatedChunk.clear();
	UpdatedChunkLock.unlock();
	return chunkIDs;
}

vector<ChunkID> WorldInteractions::getRequestedLightUpdates() {
	vector<ChunkID> columnIDs = {};
	UpdatedChunkLock.lock();
	columnIDs.insert(columnIDs.end(), LightUpdateRequest.begin(), LightUpdateRequest.end());
	LightUpdateRequest.clear();
	UpdatedChunkLock.unlock();
	return columnIDs;
}

vector<EntityProperty> WorldInteractions::getUpdatedEntities() {
	unordered_map<EntityUUID, EntityProperty> m = world->Entities.ClientGetEntityUpdate();
	vector<EntityProperty> properties = {};

	for (const auto& e : m) {
		properties.push_back(e.second);
	}

	return properties;
}

vector<EntityUUID> WorldInteractions::getRemovedEntities() {
	unordered_set<EntityUUID> m = world->Entities.getRemovedEntities();
	vector<EntityUUID> IDs = {};

	IDs.insert(IDs.end(), m.begin(), m.end());

	return IDs;
}

void WorldInteractions::requestLightUpdate(int x, int y, int z) {
	y = y / 32;

	ChunkColumnID id = getChunkID(x, y, z);
	if (RequestedLightUpdate.count(id)) {
		return;
	}

	UpdatedChunkLock.lock();
	RequestedLightUpdate.emplace(id);
	LightUpdateRequest.push_back(id);
	UpdatedChunkLock.unlock();

}

void WorldInteractions::killEntity(EntityUUID id) {
	world->Entities.RemoveEntity(id);
	if (worldLoader->checkEntityExistChunkLoader(id)) {
		worldLoader->deleteEntityChunkLoader(id);
	}
}

void WorldInteractions::update() {
	worldLoader->load();
}

void WorldInteractions::updateLighting(ChunkLightingContainer* ChunkLighting) {
	ivec3 pos = ChunkLighting->Position;

	world->getColumn(pos);
	worldLoader->replaceLightInfomation(ChunkLighting);
	ChunkID id = getChunkID(pos);

	UpdatedChunkLock.lock();
	if (!UpdatedChunk.count(id)) {
		UpdatedChunk.insert(id);
	}
	RequestedLightUpdate.erase(id);
	UpdatedChunkLock.unlock();
}

void WorldInteractions::updateLighting(vector<ChunkLightingContainer*> ChunkLighting) {
	stack<ChunkID> chunkToUpdate;

	for (const auto& chunk : ChunkLighting) {
		int x = chunk->Position.x;
		int y = chunk->Position.y;
		int z = chunk->Position.z;

		world->getColumn(ivec3(x, y, z));
		worldLoader->replaceLightInfomation(chunk);
		chunkToUpdate.push(getChunkID(x, y, z));

	}

	UpdatedChunkLock.lock();
	while (!chunkToUpdate.empty()) {
		ChunkID id = chunkToUpdate.top();
		chunkToUpdate.pop();
		if (!UpdatedChunk.count(id)) {
			UpdatedChunk.insert(id);
		}
		RequestedLightUpdate.erase(id);

	}
	UpdatedChunkLock.unlock();
}

void WorldInteractions::addChunk(Chunk* chunk) {
	world->setChunk(chunk);
	stack<ChunkID> chunkToUpdate;

	int x = chunk->Position.x;
	int y = chunk->Position.y;
	int z = chunk->Position.z;

	requestLightUpdate(x, y, z);
	chunkToUpdate.push(getChunkID(x, y, z));

	//Update neighbor chunks

	for (int side = 0; side < 6; side++) {
		int pos[3]{ x, y, z };
		pos[side >> 1] += (side & 0b1) * 2 - 1;

		if (world->checkChunk(pos[0], pos[1], pos[2])) {
			chunkToUpdate.push(getChunkID(pos[0], pos[1], pos[2]));
		}
	}

	UpdatedChunkLock.lock();
	while (!chunkToUpdate.empty()) {
		ChunkID id = chunkToUpdate.top();
		chunkToUpdate.pop();
		if (!UpdatedChunk.count(id)) {
			UpdatedChunk.insert(id);
		}

	}

	UpdatedChunkLock.unlock();
}

void WorldInteractions::addChunks(vector<Chunk*> chunks) {
	stack<ChunkID> chunkToUpdate;

	for (const auto& chunk : chunks) {
		world->setChunk(chunk);

		int x = chunk->Position.x;
		int y = chunk->Position.y;
		int z = chunk->Position.z;

		chunkToUpdate.push(getChunkID(x, y, z));

		//Update neighbor chunks

		for (int side = 0; side < 6; side++) {
			int pos[3]{ x, y, z };
			pos[side >> 1] += (side & 0b1) * 2 - 1;

			if (world->checkChunk(pos[0], pos[1], pos[2])) {
				chunkToUpdate.push(getChunkID(pos[0], pos[1], pos[2]));
			}
		}
		requestLightUpdate(x, y, z);
	}

	UpdatedChunkLock.lock();
	while (!chunkToUpdate.empty()) {
		ChunkID id = chunkToUpdate.top();
		chunkToUpdate.pop();
		if (!UpdatedChunk.count(id)) {
			UpdatedChunk.insert(id);
		}

	}
	UpdatedChunkLock.unlock();
}

Chunk* WorldInteractions::getChunk(int x, int y, int z) {
	return getChunk(getChunkID(x, y, z));
}

Chunk* WorldInteractions::getChunk(ChunkID ID) {
	if (!world->checkChunk(ID)) {
		return nullptr;
	}

	return world->getChunk(ID);
}

void WorldInteractions::setBlock(BlockID b, int x, int y, int z) {
	try {
		world->setBlock(b, x, y, z);
		UpdatedChunkLock.lock();
		if (!UpdatedChunk.count(getChunkID(x >> 4, y >> 4, z >> 4)))
			UpdatedChunk.insert(getChunkID(x >> 4, y >> 4, z >> 4));
		UpdatedChunkLock.unlock();

		int v[3]{ x % 16, y % 16, z % 16 };

		for (int side = 0; side < 3; side++) {
			int p[3]{ x >> 4, y >> 4, z >> 4 };

			int direction = 0;

			if (v[side] == 15)
				direction = 1;
			if (v[side] == 0)
				direction = -1;

			if (direction == 0) continue;

			p[side] += direction;
			UpdatedChunkLock.lock();
			if (!UpdatedChunk.count(getChunkID(p[0], p[1], p[2])))
				UpdatedChunk.insert(getChunkID(p[0], p[1], p[2]));
			UpdatedChunkLock.unlock();
		}


		requestLightUpdate(x >> 4, y >> 4, z >> 4);
	}
	catch (exception& e) {
		Logger.LogError("World", e.what());
	}
}

BlockID WorldInteractions::getBlock(int x, int y, int z) {
	try {
		return world->getBlock(x, y, z);
	}
	catch (exception& e) {
		Logger.LogError("World", e.what());
		return Blocks.AIR;
	}
}

Entity* WorldInteractions::getEntity(EntityUUID id) {
	return world->Entities.getEntity(id);
}

void WorldInteractions::addEntity(Entity& entity) {
	world->Entities.AddEntities(entity);
}

void WorldInteractions::addEntity(Entity* entity) {
	world->Entities.AddEntities(entity);
}