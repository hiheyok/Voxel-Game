#pragma once
#include "MultiEntityRender.h"
#include "../../Level/Timer/Timer.h"
#include <thread>

class EntityRendererUpdater {
    MultiEntityRenderer* renderer_;
    Dimension* dimension_;

    bool stop_ = true;

    void UpdaterThread();

    std::thread update_thread_;

    Timer* server_time_;
public:
    EntityRendererUpdater();
    void SetEntityRenderer(MultiEntityRenderer* render, Timer* time);
    void Start(Dimension* dimension);
    void Stop();
};