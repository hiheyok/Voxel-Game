#pragma once
#include "../Level.h"
#include "../Timer/Timer.h"
#include "../../Utils/Clock.h"
#include <thread>
#include "../../Core/Options/Option.h"

struct ServerSettings {
    int gen_thread_count_ = 8;
    int light_engine_thread_count_ = 2;
    int tick_rate_ = 20;
    int horizontal_ticking_distance_ = 10;
    int vertical_ticking_distance_ = 8;
};

//This manages the input/output system of the world
//This also manages the ticking system
class Server {
private:
    bool stop_ = true; // TODO: Rename
    std::thread main_server_loop_;
    Timer time_; // TODO: Rename
    double mspt_ = 0.0;
public:
    Level level_;

    ServerSettings settings_;

    Server() {

    }

    size_t GetChunkCount();

    double GetMSPT();// TODO: Rename

    Timer* GetTimer();

    std::vector<EntityProperty> GetUpdatedEntities();// TODO: Rename

    std::vector<EntityUUID> GetRemovedEntities();// TODO: Rename

    bool CheckEntityOnGround(EntityUUID id);

    void Join(Entity& entity);// TODO: Rename

    std::vector<ChunkPos> GetUpdatedChunkPos();

    Chunk* GetChunk(const ChunkPos& id);

    BlockID GetBlock(const BlockPos& pos);

    bool GetRayIntersection(Ray& ray);

    glm::vec3 GetEntityCollusionTime(EntityUUID entity);

    void StartServer(ServerSettings serverSettings);

    void Stop();

    void Loop();

    void Tick();

    void SendEvent(const Event::Event& event);
};