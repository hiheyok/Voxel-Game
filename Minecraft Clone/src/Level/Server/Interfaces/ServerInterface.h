#pragma once
#include <vector>
#include "../../../Utils/Containers/ConcurrentQueue.h"
#include "../Networking/Packet.h"
#include <string>

/*
* Abstraction for client to server interfacing
*/
class ServerInterface {
protected:
    // Server -> Client queues
    ConcurrentQueue<Packet::BlockUpdate> block_update_queue_;
    ConcurrentQueue<Packet::EntitySpawn> entity_spawn_queue_;
    ConcurrentQueue<Packet::EntityUpdate> entity_update_queue_;
    ConcurrentQueue<Packet::EntityDespawn> entity_despawn_queue_;
    ConcurrentQueue<Packet::ChunkUpdateData> chunk_update_queue_;
public:
    virtual ~ServerInterface() = default;

    virtual void SendPlayerAction(const Packet::PlayerAction& input) = 0;
    virtual void SendBlockAction(const Packet::BlockAction& action) = 0;

    virtual bool IsConnected() const = 0;
    virtual void Connect(const std::string& address, int port) = 0; // For external server
    virtual void Disconnect() = 0;
    virtual void Update() = 0;

    bool PollBlockUpdates(std::vector<Packet::BlockUpdate>& outUpdates) {
        size_t prevSize = outUpdates.size();
        block_update_queue_.PopAll(outUpdates);
        return prevSize != outUpdates.size();
    }

    bool PollEntitySpawns(std::vector<Packet::EntitySpawn>& outSpawns) {
        size_t prevSize = outSpawns.size();
        entity_spawn_queue_.PopAll(outSpawns);
        return prevSize != outSpawns.size();
    }

    bool PollEntityUpdates(std::vector<Packet::EntityUpdate>& outUpdates) {
        size_t prevSize = outUpdates.size();
        entity_update_queue_.PopAll(outUpdates);
        return prevSize != outUpdates.size();
    }

    bool PollEntityDespawns(std::vector<Packet::EntityDespawn>& outDespawns) {
        size_t prevSize = outDespawns.size();
        entity_despawn_queue_.PopAll(outDespawns);
        return prevSize != outDespawns.size();
    }

    bool PollChunkUpdates(std::vector<Packet::ChunkUpdateData>& outUpdates) {
        size_t prevSize = outUpdates.size();
        chunk_update_queue_.PopAll(outUpdates);
        return prevSize != outUpdates.size();
    }

    
};