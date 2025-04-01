#pragma once
#include "Level/Chunk/ChunkRawData.h"
#include "Level/Chunk/Lighting/ChunkLighting.h"
#include "Core/Typenames.h"

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