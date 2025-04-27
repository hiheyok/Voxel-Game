
#ifndef CORE_POSITION_POSITION_TYPE_H
#define CORE_POSITION_POSITION_TYPE_H

#include "Core/Position/Position.h"

class ChunkPos;
class RegionPos;
class BlockPos;

class RegionPos : public Position<RegionPos>{
public:
    using Position<RegionPos>::Position;
};

class ChunkPos : public Position<ChunkPos>{
public:
    using Position<ChunkPos>::Position;

    operator RegionPos() const noexcept;
    size_t GetIndex() const noexcept;
};

class BlockPos : public Position<BlockPos>{
public:
    using Position<BlockPos>::Position;

    operator RegionPos() const noexcept;
    operator ChunkPos() const noexcept;

    size_t GetIndex() const noexcept;
    BlockPos GetLocalPos() const noexcept;
};


#endif