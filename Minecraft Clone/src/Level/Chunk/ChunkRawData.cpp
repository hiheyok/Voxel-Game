// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Chunk/ChunkRawData.h"

#include "Core/Position/PositionTypes.h"
#include "Level/Chunk/Palette.h"
#include "Level/Light/LightStorage.h"

ChunkRawData::ChunkRawData(const Palette& chunkData,
                           const LightStorage& sky_light,
                           const LightStorage& block_light, ChunkPos pos)
    : chunk_data_{chunkData},
      sky_light_{sky_light},
      block_light_{block_light},
      pos_{pos} {}
ChunkRawData::ChunkRawData(const ChunkRawData&) = default;
ChunkRawData::ChunkRawData(ChunkRawData&&) = default;
ChunkRawData::~ChunkRawData() = default;

ChunkRawData& ChunkRawData::operator=(const ChunkRawData&) = default;
ChunkRawData& ChunkRawData::operator=(ChunkRawData&&) = default;
