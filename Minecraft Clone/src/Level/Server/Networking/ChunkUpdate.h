#pragma once
#include "../../Chunk/ChunkRawData.h"
#include "../../Chunk/Lighting/ChunkLighting.h"
#include "../../Typenames.h"

namespace ChunkUpdatePacket {

    enum PacketType {
        ADD_CHUNK, LIGHT_UPDATE, DELETE_CHUNK
    };

    struct AddChunk {
        ChunkRawData chunk_;
    };

    struct LightUpdate {
        ChunkLightingContainer light_;
    };

    struct DeleteChunk {
        ChunkPos pos_;
    };
}