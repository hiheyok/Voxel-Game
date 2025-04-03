#pragma once
#include "Level/Chunk/ChunkRawData.h"
#include "Level/Chunk/Lighting/LightStorage.h"
#include "Core/Typenames.h"

namespace ChunkUpdatePacket {

    enum PacketType {
        ADD_CHUNK, LIGHT_UPDATE, DELETE_CHUNK
    };

    struct AddChunk {
        ChunkRawData chunk_;
    };

    struct LightUpdate {
        LightStorage light_;
    };

    struct DeleteChunk {
        ChunkPos pos_;
    };
}