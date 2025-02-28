#include "WorldInteractions.h"

#include <intrin.h>

using namespace std;
using namespace glm;


void WorldInteractions::UseTallGeneration() {
	worldLoader->tallGeneration = true;
}

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

vector<ChunkPos> WorldInteractions::getUpdatedChunkPos() {
	vector<ChunkPos> chunkIDs = {};
	
	UpdatedChunkLock.lock();
	chunkIDs.insert(chunkIDs.end(), UpdatedChunk.begin(), UpdatedChunk.end());
	UpdatedChunk.clear();
	UpdatedChunkLock.unlock();
	return chunkIDs;
}

vector<ChunkPos> WorldInteractions::getRequestedLightUpdates() {
	vector<ChunkPos> columnIDs = {};
	UpdatedChunkLock.lock();
	columnIDs.insert(columnIDs.end(), LightUpdateRequest.begin(), LightUpdateRequest.end());
	LightUpdateRequest.clear();
	UpdatedChunkLock.unlock();
	return columnIDs;
}

vector<EntityProperty> WorldInteractions::getUpdatedEntities() {
	FastHashMap<EntityUUID, EntityProperty> m = world->Entities.ClientGetEntityUpdate();
	vector<EntityProperty> properties = {};

	for (const auto& e : m) {
		properties.push_back(e.second);
	}

	return properties;
}

vector<EntityUUID> WorldInteractions::getRemovedEntities() {
	FastHashSet<EntityUUID> m = world->Entities.getRemovedEntities();
	vector<EntityUUID> IDs = {};

	IDs.insert(IDs.end(), m.begin(), m.end());

	return IDs;
}

void WorldInteractions::requestLightUpdate(const ChunkPos& pos) {
	if (RequestedLightUpdate.count(pos)) {
		return;
	}

	UpdatedChunkLock.lock();
	RequestedLightUpdate.emplace(pos);
	LightUpdateRequest.push_back(pos);
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
	const ChunkPos& pos = ChunkLighting->position_;

	world->getColumn(ChunkLighting->position_);
	worldLoader->replaceLightInfomation(ChunkLighting);

	UpdatedChunkLock.lock();
	if (!UpdatedChunk.count(pos)) {
		UpdatedChunk.insert(pos);
	}

	RequestedLightUpdate.erase(pos);
	UpdatedChunkLock.unlock();
}

void WorldInteractions::updateLighting(vector<ChunkLightingContainer*> ChunkLighting) {
	stack<ChunkPos> chunkToUpdate;

	for (const auto& chunk : ChunkLighting) {
		world->getColumn(chunk->position_);
		worldLoader->replaceLightInfomation(chunk);
		chunkToUpdate.push(chunk->position_);
	}

	UpdatedChunkLock.lock();
	while (!chunkToUpdate.empty()) {
		ChunkPos pos = chunkToUpdate.top();
		chunkToUpdate.pop();
		if (!UpdatedChunk.count(pos)) {
			UpdatedChunk.insert(pos);
		}
		// TODO: Fix this with Chunk Column ID
		RequestedLightUpdate.erase(pos);

	}
	UpdatedChunkLock.unlock();
}

void WorldInteractions::addChunk(Chunk* chunk) {
	world->setChunk(chunk);
	stack<ChunkPos> chunkToUpdate;

	const ChunkPos& position = chunk->position_;

	requestLightUpdate(position);
	chunkToUpdate.push(position);

	//Update neighbor chunks

	for (int side = 0; side < 6; side++) {
		ChunkPos neighborPos = position;
		neighborPos.incrementSide(side, 1);

		if (world->checkChunk(neighborPos)) {
			chunkToUpdate.push(neighborPos);
		}
	}

	UpdatedChunkLock.lock();
	while (!chunkToUpdate.empty()) {
		ChunkPos pos = chunkToUpdate.top();
		chunkToUpdate.pop();
		if (!UpdatedChunk.count(pos)) {
			UpdatedChunk.insert(pos);
		}

	}

	UpdatedChunkLock.unlock();
}

void WorldInteractions::addChunks(vector<Chunk*> chunks) {
	vector<ChunkPos> chunkToUpdate;
	FastHashSet<ChunkPos> duplicatedInputs;

	for (const auto& chunk : chunks) {
		world->setChunk(chunk);

		const ChunkPos& position = chunk->position_;

		chunkToUpdate.push_back(position);

		//Update neighbor chunks

		for (int side = 0; side < 6; side++) {
			ChunkPos neighborPos = position;
			neighborPos.incrementSide(side, 1);
			
			if (!duplicatedInputs.count(neighborPos) && world->checkChunk(neighborPos)) {
				chunkToUpdate.push_back(neighborPos);
				duplicatedInputs.insert(neighborPos);
			}
		}
		requestLightUpdate(position);
	}

	UpdatedChunkLock.lock();
	for (const ChunkPos& id : chunkToUpdate) {
		if (!UpdatedChunk.count(id)) {
			UpdatedChunk.insert(id);
		}

	}
	UpdatedChunkLock.unlock();
	chunkToUpdate.clear();
}

Chunk* WorldInteractions::getChunk(const ChunkPos& pos) const {
	if (!world->checkChunk(pos)) {
		return nullptr;
	}

	return world->getChunk(pos);
}

void WorldInteractions::setBlock(BlockID b, const BlockPos& bpos) {
	//UpdatedChunkLock.lock();
	try {
		int x = bpos.x;
		int y = bpos.y;
		int z = bpos.z;

		ChunkPos pos = ChunkPos{ x >> 4, y >> 4, z >> 4 };

		world->setBlock(b, bpos);
		if (!UpdatedChunk.count(pos)) {
			UpdatedChunk.insert(pos);
		}
		
		int v[3]{ x % 16, y % 16, z % 16 };

		for (int axis = 0; axis < 3; axis++) {
			ChunkPos p = pos;

			int direction = 0;

			if (v[axis] == 15)
				direction = 1;
			if (v[axis] == 0)
				direction = -1;

			if (direction == 0) continue;

			p[axis] += direction;
			if (!UpdatedChunk.count(p))
				UpdatedChunk.insert(p);
		}


		requestLightUpdate(pos);
	}
	catch (exception& e) {
		//UpdatedChunkLock.unlock();
		Logger.LogError("World Exception", e.what());
	}
	//UpdatedChunkLock.unlock();
}

BlockID WorldInteractions::getBlock(const BlockPos& pos) {
	try {
		return world->getBlock(pos);
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