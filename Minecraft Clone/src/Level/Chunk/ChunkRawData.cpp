#include "Level/Chunk/ChunkRawData.h"

ChunkRawData::ChunkRawData() = default;
ChunkRawData::ChunkRawData(const Palette& chunkData,
                           const LightStorage& lightData, ChunkPos pos)
    : chunk_data_{chunkData}, lighting_data_{lightData}, pos_{pos} {};
ChunkRawData::ChunkRawData(const ChunkRawData&) = default;
ChunkRawData::ChunkRawData(ChunkRawData&&) = default;
ChunkRawData::~ChunkRawData() = default;

ChunkRawData& ChunkRawData::operator=(const ChunkRawData&) = default;
ChunkRawData& ChunkRawData::operator=(ChunkRawData&&) = default;
