#pragma once
#include <memory>

class Dimension;

class Level {
   public:
    std::unique_ptr<Dimension> main_world_;

    Level();
    ~Level();

    void Start(int worldGenThreadCount, int light_engine_thread_count_);

    void Stop();

    void updateDimensions();
};