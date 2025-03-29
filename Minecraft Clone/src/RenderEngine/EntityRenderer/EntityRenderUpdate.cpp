#include "EntityRenderUpdate.h"
#include "../../Utils/Clock.h"
#include "../../Level/Dimension/Dimension.h"

EntityRendererUpdater::EntityRendererUpdater() : stop_{ true }, dimension_{ nullptr } {

}

void EntityRendererUpdater::SetEntityRenderer(MultiEntityRenderer* render, Timer* time) {
    renderer_ = render;
    server_time_ = time;
}

void EntityRendererUpdater::UpdaterThread() {
    while (!stop_) {
        std::vector<EntityProperty> UpdatedEntities = dimension_->world_interactions_.GetUpdatedEntities();
        std::vector<EntityUUID> RemovedEntities = dimension_->world_interactions_.GetRemovedEntities();

        for (auto& entity : UpdatedEntities) {
            renderer_->AddEntity(entity);

        }
        for (auto& entity : RemovedEntities) {
            renderer_->RemoveEntity(entity);
        }
        
        
        timerSleep(0.05);
    }
}

void EntityRendererUpdater::Start(Dimension* dimension) {
    dimension_ = dimension;
    stop_ = false;
    update_thread_ = std::thread(&EntityRendererUpdater::UpdaterThread, this);
}

void EntityRendererUpdater::Stop() {
    stop_ = true;
    update_thread_.join();
}

