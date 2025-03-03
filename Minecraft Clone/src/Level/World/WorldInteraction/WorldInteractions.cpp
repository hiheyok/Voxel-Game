#include "WorldInteractions.h"

#include <intrin.h>

using namespace std;
using namespace glm;


void WorldInteractions::UseTallGeneration() {
	worldLoader_->tall_generation_ = true;
}

void WorldInteractions::init(World* w, WorldParameters parameters) {
	world = w;
	settings_ = parameters;
	worldLoader_ = new WorldLoader(w, parameters);
	collusions_.Initialize(static_cast<WorldAccess*>(w));
}

void WorldInteractions::summonEntity(Entity& entity) {
	if (world == nullptr) throw exception("Cannot summon entity. World is null");
	world->entities_.AddEntities(entity);

	if (entity.properties_.is_chunk_loader_)
		worldLoader_->AddEntityChunkLoader(entity.properties_.entity_uuid_);
}

vector<ChunkPos> WorldInteractions::GetUpdatedChunkPos() {
	std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
	vector<ChunkPos> chunkIDs = {};
	chunkIDs.insert(chunkIDs.end(), updated_chunk_.begin(), updated_chunk_.end());
	updated_chunk_.clear();

	return chunkIDs;
}

vector<ChunkPos> WorldInteractions::GetRequestedLightUpdates() {
	std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
	vector<ChunkPos> columnIDs = std::move(light_update_request_);
	light_update_request_.clear();
	return columnIDs;
}

vector<EntityProperty> WorldInteractions::getUpdatedEntities() {
	FastHashMap<EntityUUID, EntityProperty> m = world->entities_.ClientGetEntityUpdate();
	vector<EntityProperty> properties = {};

	for (const auto& e : m) {
		properties.push_back(e.second);
	}

	return properties;
}

vector<EntityUUID> WorldInteractions::getRemovedEntities() {
	FastHashSet<EntityUUID> m = world->entities_.getRemovedEntities();
	vector<EntityUUID> ids = {};
	ids.insert(ids.end(), m.begin(), m.end());
	return ids;
}

void WorldInteractions::RequestLightUpdate(const ChunkPos& pos) {
	if (requested_light_update_.count(pos)) {
		return;
	}

	std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
	requested_light_update_.emplace(pos);
	light_update_request_.push_back(pos);

}

void WorldInteractions::KillEntity(EntityUUID id) {
	world->entities_.RemoveEntity(id);
	if (worldLoader_->CheckEntityExistChunkLoader(id)) {
		worldLoader_->DeleteEntityChunkLoader(id);
	}
}

void WorldInteractions::Update() {
	worldLoader_->Load();
}

void WorldInteractions::UpdateLighting(std::shared_ptr<ChunkLightingContainer> chunkLighting) {
	ChunkPos pos = chunkLighting->position_;

	world->GetColumn(chunkLighting->position_);
	worldLoader_->ReplaceLightInfomation(std::move(chunkLighting));

	std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
	if (!updated_chunk_.count(pos)) {
		updated_chunk_.insert(pos);
	}

	requested_light_update_.erase(pos);
}

void WorldInteractions::UpdateLighting(vector<std::shared_ptr<ChunkLightingContainer>> chunkLighting) {
	stack<ChunkPos> chunkToUpdate;

	for (auto& chunk : chunkLighting) {
		chunkToUpdate.push(chunk->position_);
		worldLoader_->ReplaceLightInfomation(std::move(chunk));
	}

	std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };

	while (!chunkToUpdate.empty()) {
		ChunkPos pos = chunkToUpdate.top();
		chunkToUpdate.pop();
		if (!updated_chunk_.count(pos)) {
			updated_chunk_.insert(pos);
		}
		// TODO: Fix this with Chunk Column ID
		requested_light_update_.erase(pos);

	}
}

void WorldInteractions::AddChunk(Chunk* chunk) {
	world->SetChunk(chunk);
	stack<ChunkPos> chunkToUpdate;

	const ChunkPos& position = chunk->position_;

	RequestLightUpdate(position);
	chunkToUpdate.push(position);

	//Update neighbor chunks

	for (int side = 0; side < 6; side++) {
		ChunkPos neighborPos = position;
		neighborPos.incrementSide(side, 1);

		if (world->CheckChunk(neighborPos)) {
			chunkToUpdate.push(neighborPos);
		}
	}

	std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };

	while (!chunkToUpdate.empty()) {
		ChunkPos pos = chunkToUpdate.top();
		chunkToUpdate.pop();
		if (!updated_chunk_.count(pos)) {
			updated_chunk_.insert(pos);
		}

	}
}

void WorldInteractions::AddChunks(vector<Chunk*> chunks) {
	vector<ChunkPos> chunkToUpdate;
	FastHashSet<ChunkPos> duplicatedInputs;

	for (const auto& chunk : chunks) {
		world->SetChunk(chunk);

		const ChunkPos& position = chunk->position_;

		chunkToUpdate.push_back(position);

		//Update neighbor chunks

		for (int side = 0; side < 6; side++) {
			ChunkPos neighborPos = position;
			neighborPos.incrementSide(side, 1);
			
			if (!duplicatedInputs.count(neighborPos) && world->CheckChunk(neighborPos)) {
				chunkToUpdate.push_back(neighborPos);
				duplicatedInputs.insert(neighborPos);
			}
		}
		RequestLightUpdate(position);
	}

	{
		std::lock_guard<std::mutex> lock{ updated_chunk_lock_ };
		for (const ChunkPos& pos : chunkToUpdate) {
			if (!updated_chunk_.count(pos)) {
				updated_chunk_.insert(pos);
			}

		}
	}
	chunkToUpdate.clear();
}

Chunk* WorldInteractions::GetChunk(const ChunkPos& pos) const {
	if (!world->CheckChunk(pos)) {
		return nullptr;
	}

	return world->GetChunk(pos);
}

void WorldInteractions::SetBlock(BlockID b, const BlockPos& bpos) {
	//UpdatedChunkLock.lock();
	try {
		int x = bpos.x;
		int y = bpos.y;
		int z = bpos.z;

		ChunkPos pos = ChunkPos{ x >> 4, y >> 4, z >> 4 };

		world->SetBlock(b, bpos);
		if (!updated_chunk_.count(pos)) {
			updated_chunk_.insert(pos);
		}
		
		int v[3]{ x % 16, y % 16, z % 16 };

		for (int axis_ = 0; axis_ < 3; axis_++) {
			ChunkPos p = pos;

			int direction = 0;

			if (v[axis_] == 15)
				direction = 1;
			if (v[axis_] == 0)
				direction = -1;

			if (direction == 0) continue;

			p[axis_] += direction;
			if (!updated_chunk_.count(p))
				updated_chunk_.insert(p);
		}


		RequestLightUpdate(pos);
	}
	catch (exception& e) {
		g_logger.LogError("World Exception", e.what());
	}
}

BlockID WorldInteractions::GetBlock(const BlockPos& pos) {
	try {
		return world->GetBlock(pos);
	}
	catch (exception& e) {
		g_logger.LogError("World", e.what());
		return g_blocks.AIR;
	}
}

Entity* WorldInteractions::GetEntity(EntityUUID id) {
	return world->entities_.GetEntity(id);
}

void WorldInteractions::AddEntity(Entity& entity) {
	world->entities_.AddEntities(entity);
}

void WorldInteractions::AddEntity(Entity* entity) {
	world->entities_.AddEntities(entity);
}