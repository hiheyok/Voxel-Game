#include "EntityRenderUpdate.h"
#include "../../Utils/Clock.h"


void EntityRendererUpdater::SetEntityRenderer(MultiEntityRenderer* render, Timer* time) {
	renderer = render;
	ServerTime = time;
}

void EntityRendererUpdater::UpdaterThread() {
	while (!stop) {
		World* world = static_cast<World*>(Block::WorldPTR);

		world->EntityUpdateLock.lock();
		std::unordered_map<EntityUUID, EntityProperty> UpdatedEntities = world->EntityUpdated;
		std::unordered_set<EntityUUID> RemovedEntities = world->RemovedEntity;
		world->EntityUpdated.clear();
		world->RemovedEntity.clear();
		world->EntityUpdateLock.unlock();

		for (auto& entity : UpdatedEntities) {
			renderer->AddEntity(entity.second);

		}
		for (auto& entity : RemovedEntities) {
			renderer->RemoveEntity(entity);
		}

		
		
		timerSleep(0);
	}
}

void EntityRendererUpdater::Start() {
	stop = false;
	UpdateThread = std::thread(&EntityRendererUpdater::UpdaterThread, this);
}

void EntityRendererUpdater::Stop() {
	stop = true;
	UpdateThread.join();
}

