#pragma once
#include <glm/vec3.hpp>

#include "Level/Item/ItemStack.h"
#include "Core/Typenames.h"

namespace PlayerPacket {

    enum PacketType {
        GET_ITEM, DESTROY_BLOCK, PLACE_BLOCK, MOVE
    };

    struct PlayerGetItem {
        ItemStack item_;
        int slot_;
        PlayerGetItem() = default;
    };

    struct PlayerDestroyBlock {
        BlockPos pos_;
        PlayerDestroyBlock() = default;
    };

    struct PlayerPlaceBlock {
        BlockPos pos_;
        BlockID block_;
        PlayerPlaceBlock() = default;
    };

    struct PlayerMove {
        glm::vec3 pos_, vel_, acc_;
        PlayerMove() = default;
    };
}