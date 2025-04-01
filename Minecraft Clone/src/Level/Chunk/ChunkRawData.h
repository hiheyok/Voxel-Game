#pragma once
#include "Level/Chunk/Palette.h"
#include "Level/Chunk/Lighting/ChunkLighting.h"

struct ChunkRawData {
    Palette chunk_data_;
    ChunkLightingContainer lighting_data_;
    ChunkPos pos_;

    ChunkRawData() = default;
    ChunkRawData(const Palette& chunkData, const ChunkLightingContainer& lightData, const ChunkPos& pos) : chunk_data_{ chunkData }, lighting_data_{ lightData }, pos_{ pos } {};
    ChunkRawData(const ChunkRawData&) = default;
    ChunkRawData(ChunkRawData&&) = default;
    ~ChunkRawData() = default;

    ChunkRawData& operator=(const ChunkRawData&) = default;
    ChunkRawData& operator=(ChunkRawData&&) = default;
};