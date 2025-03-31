#pragma once
#include <thread>
#include <vector>
#include <glm/vec3.hpp>

#include "../Typenames.h"

// TODO: Currently default to main dimension for now (change this later)
class ClientInterface;
class InternalInterface;
class Timer;
class Level;
class Chunk;
class Entity;

struct Ray;
struct EntityProperty;

struct Event;

struct ServerSettings {
    size_t gen_thread_count_ = 8;
    size_t light_engine_thread_count_ = 2;
    size_t tick_rate_ = 20;
    size_t horizontal_ticking_distance_ = 10;
    size_t vertical_ticking_distance_ = 8;

    ServerSettings() = default;
    ServerSettings(const ServerSettings&) = default;
    ServerSettings(ServerSettings&&) = default;
    ~ServerSettings() = default;
};

//This manages the input/output system of the world
//This also manages the ticking system
class Server {
private:
    bool stop_ = true; // TODO: Rename
    std::thread main_server_loop_;
    std::unique_ptr<Timer> time_; // TODO: Rename
    double mspt_ = 0.0;
public:
    std::unique_ptr<Level> level_ = nullptr;
    std::unique_ptr<ServerSettings> settings_ = nullptr;
    ClientInterface* client_interface_ = nullptr;

    Server();
    ~Server();

    size_t GetChunkCount();

    double GetMSPT();

    Timer* GetTimer();

    std::vector<EntityProperty> GetUpdatedEntities();

    std::vector<EntityUUID> GetRemovedEntities();

    bool CheckEntityOnGround(EntityUUID id);

    void Join(Entity& entity);

    std::vector<ChunkPos> GetUpdatedChunkPos();

    Chunk* GetChunk(const ChunkPos& id);

    BlockID GetBlock(const BlockPos& pos);

    bool GetRayIntersection(Ray& ray);

    glm::vec3 GetEntityCollusionTime(EntityUUID entity);

    void StartServer(ServerSettings serverSettings);

    void Stop();

    void Loop();

    void Tick();

    void SendEvent(const Event& event);

    void SetInternalConnection(InternalInterface* conn);

    void ProcessPacket();
};