#pragma once
#include "../../../Utils/Containers/ConcurrentQueue.h"
#include "../Networking/Packet.h"

/*
* Abstraction for server to client interfacing
*/

class ClientInterface {
protected:
    // Client -> Server queues
    ConcurrentQueue<Packet::PlayerAction> player_action_queue_;
    ConcurrentQueue<Packet::BlockAction> block_action_queue_;
public:
    virtual void SendBlockUpdate(const Packet::BlockUpdate& update) = 0;
    virtual void SendEntitySpawns(const Packet::EntitySpawn& entitySpawn) = 0;
    virtual void SendEntityUpdate(const Packet::EntityUpdate& update) = 0;
    virtual void SendEntityDespawns(const Packet::EntityDespawn& entityDespawn) = 0;
    virtual void SendChunkUpdates(const Packet::ChunkUpdateData& update) = 0;

    bool PollClientPlayerAction(std::vector<Packet::PlayerAction>& playerAction) {
        size_t prevSize = playerAction.size();
        player_action_queue_.PopAll(playerAction);
        return prevSize != playerAction.size();
    }

    bool PollClientBlockAction(std::vector<Packet::BlockAction>& blockAction) {
        size_t prevSize = blockAction.size();
        block_action_queue_.PopAll(blockAction);
        return prevSize != blockAction.size();
    }

    
};