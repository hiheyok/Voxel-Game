#pragma once
#include "Core/Typenames.h"
#include "Core/Networking/Packet.h"
#include "Core/Stats/ServerStats.h"
#include "Utils/Containers/ConcurrentQueue.h"

/*
* Abstraction for server to client interfacing
*/

class ClientInterface {
public:
    virtual void SendBlockUpdate(const Packet::BlockUpdate& update) = 0;
    virtual void SendEntityUpdate(const Packet::EntityUpdate& update) = 0;
    virtual void SendChunkUpdates(const Packet::ChunkUpdateData& update) = 0;
    virtual void SendServerStats(const ServerStats& stats) = 0;

    virtual void SendTimeLastTick() = 0;

    ClientInterface() = default;

    size_t PollClientPlayerAction(std::vector<Packet::PlayerAction>& playerAction) {
        size_t prevSize = playerAction.size();
        player_action_queue_.PopAll(playerAction);
        return playerAction.size() - prevSize;
    }

    EntityUUID GetPlayerUUID() const { 
        return player_uuid_; 
    }

    void SetPlayerUUID(EntityUUID playerUUID) { 
        player_uuid_ = playerUUID;
    }
protected:
    // Client -> Server queues
    ConcurrentQueue<Packet::PlayerAction> player_action_queue_;
    EntityUUID player_uuid_;
};