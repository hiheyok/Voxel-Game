#pragma once
#include "../../Chunk/ChunkRawData.h"
#include "../../Chunk/Lighting/ChunkLighting.h"
#include "../../Typenames.h"

namespace ChunkUpdatePacket {
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