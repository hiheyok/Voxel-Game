#include "EntityRenderUpdate.h"
#include "../../Utils/Clock.h"
#include "../../Level/Dimension/Dimension.h"

void EntityRendererUpdater::SetEntityRenderer(MultiEntityRenderer* render, Timer* time) {
    renderer_ = render;
    server_time_ = time;
}

void EntityRendererUpdater::UpdaterThread() {
    while (!stop_) {
        Dimension* world = static_cast<Dimension*>(Block::dimension_ptr_);

        std::vector<EntityProperty> UpdatedEntities = world->world_interactions_.GetUpdatedEntities();
        std::vector<EntityUUID> RemovedEntities = world->world_interactions_.GetRemovedEntities();

        for (auto& entity : UpdatedEntities) {
            renderer_->AddEntity(entity);

        }
        for (auto& entity : RemovedEntities) {
            renderer_->RemoveEntity(entity);
        }
        
        
        timerSleep(0.05);
    }
}

void EntityRendererUpdater::Start() {
    stop_ = false;
    update_thread_ = std::thread(&EntityRendererUpdater::UpdaterThread, this);
}

void EntityRendererUpdater::Stop() {
    stop_ = true;
    update_thread_.join();
}

