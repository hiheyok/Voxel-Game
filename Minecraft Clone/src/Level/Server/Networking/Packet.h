#pragma once
#include <variant>

#include "PlayerAction.h"
#include "EntityUpdate.h"
#include "BlockUpdate.h"
#include "ChunkUpdate.h"

namespace Packet {

    struct PlayerAction {
        PlayerPacket::PlayerPacketType type_;
        std::variant<
            PlayerPacket::PlayerDestroyBlock,
            PlayerPacket::PlayerGetItem,
            PlayerPacket::PlayerMove,
            PlayerPacket::PlayerPlaceBlock,
            std::monostate> packet_;
    };

    struct EntityUpdate {
        std::variant<
            EntityUpdatePacket::EntityDespawn,
            EntityUpdatePacket::EntityInventoryUpdate,
            EntityUpdatePacket::EntityMove,
            EntityUpdatePacket::EntitySpawn,
            std::monostate> packet_;

        template <class T>
        EntityUpdate(const T& packet) {
            packet_ = packet;
        }
    };

    struct BlockUpdate {
        std::variant<
            BlockUpdatePacket::BlockUpdate,
            std::monostate> packet_;
    };

    struct ChunkUpdateData {
        std::variant<
            ChunkUpdatePacket::AddChunk,
            ChunkUpdatePacket::DeleteChunk,
            ChunkUpdatePacket::LightUpdate,
            std::monostate> packet_;

        template <class T>
        ChunkUpdateData(const T& packet) {
            packet_ = packet;
        }
    };
}
