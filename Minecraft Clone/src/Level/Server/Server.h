#pragma once
#include <thread>
#include <vector>
#include <glm/vec3.hpp>

#include "Core/Typenames.h"

// TODO: Currently default to main dimension for now (change this later)
class ClientInterface;
class InternalInterface;
class Timer;
class Level;
class Chunk;

struct Entity;
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
public:
    std::unique_ptr<Level> level_ = nullptr;
    std::unique_ptr<ServerSettings> settings_ = nullptr;
    ClientInterface* client_interface_ = nullptr;

    Server();
    ~Server();

    size_t GetChunkCount();

    double GetMSPT();

    Timer* GetTimer();

    void StartServer(ServerSettings serverSettings);

    void Stop();

    void Loop();

    void Tick();

    void SendEvent(const Event& event);

    // This will cause the player to join and return a uuid for the player
    EntityUUID SetInternalConnection(InternalInterface* conn);
private:
    bool stop_ = true; // TODO: Rename
    std::thread main_server_loop_;
    std::unique_ptr<Timer> time_; // TODO: Rename
    double mspt_ = 0.0;

    void ProcessPacket();
    void ProcessPlayerPackets(ClientInterface* receiver);
    void SendPacket();
    void SendEntityUpdatePacket(ClientInterface* receiver);
    void SendChunkUpdatePacket(ClientInterface* receiver);
};