#pragma once
#include <vector>
#include <glm/vec3.hpp>

#include "../../Typenames.h"

class Player;
class Timer;
class Chunk;
class Server;

struct Ray;
struct EntityProperty;

namespace Event {
    struct Event;
}

class InternalServer {
private:
    Player* player;
public:
    Server* server;

    void startInternalServer(Player* p);

    void Stop();

    glm::vec3 GetPlayerCollusionTimes();

    Chunk* GetChunk(const ChunkPos& chunk);

    Timer* GetTickClock();

    double GetMSPT();

    void SetPlayer(Player player);

    bool CheckPlayerOnGround();

    size_t GetChunkCount();

    std::vector<ChunkPos> GetUpdatedChunks();

    std::vector<EntityProperty> GetUpdatedEntities();

    std::vector<EntityUUID> GetRemovedEntities();

    bool CheckRayIntersection(Ray& ray);

    BlockID GetBlock(const BlockPos& pos);

    void SendEvent(Event::Event pEventIn);

};