#pragma once
#include <thread>
#include <memory>

class Timer;
class Dimension;
class MultiEntityRenderer;

class EntityRendererUpdater {
public:
    EntityRendererUpdater();
    ~EntityRendererUpdater();
    void SetEntityRenderer(MultiEntityRenderer* render, Timer* time);
    void Start(Dimension* dimension);
    void Stop();
private:
    void UpdaterThread();

    MultiEntityRenderer* renderer_;
    Dimension* dimension_;
    Timer* server_time_;

    bool stop_ = true;
    std::thread update_thread_;
};