#include "EntityRenderUpdate.h"
#include "../../Utils/Clock.h"
#include "../../Level/Dimension/Dimension.h"

void EntityRendererUpdater::SetEntityRenderer(MultiEntityRenderer* render, Timer* time) {
	renderer = render;
	ServerTime = time;
}

void EntityRendererUpdater::UpdaterThread() {
	while (!stop) {
		Dimension* world = static_cast<Dimension*>(Block::DimensionPTR);

		std::vector<EntityProperty> UpdatedEntities = world->worldInteractions.getUpdatedEntities();
		std::vector<EntityUUID> RemovedEntities = world->worldInteractions.getRemovedEntities();

		for (auto& entity : UpdatedEntities) {
			renderer->AddEntity(entity);

		}
		for (auto& entity : RemovedEntities) {
			renderer->RemoveEntity(entity);
		}
		
		
		timerSleep(0.05);
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

