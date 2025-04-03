#pragma once
#include <memory>

class Dimension;
class LevelLoader;

class Level {
public:
    std::unique_ptr<Dimension> main_world_;
    std::unique_ptr<LevelLoader> level_loader_;

    Level();
    ~Level();

    void Start(int worldGenThreadCount, int light_engine_thread_count_);

    void Stop();

    void updateDimensions();
};