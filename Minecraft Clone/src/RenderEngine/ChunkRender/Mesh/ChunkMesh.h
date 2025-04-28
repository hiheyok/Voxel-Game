#pragma once
#include <array>
#include <bitset>
#include <glm/ext.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "Core/Typenames.h"

class Chunk;

struct Cuboid;
struct BlockFace;
struct BlockModel;
// TODO: refactor this

namespace Mesh {
struct ChunkVertexData {
    ChunkVertexData() = default;
    ~ChunkVertexData() = default;

    std::vector<uint32_t> solid_vertices_;
    std::vector<uint32_t> transparent_vertices_;
    std::vector<uint32_t> fluid_vertices_buffer_;

    ChunkPos position_;
};

class ChunkMeshData {
   public:
    ChunkMeshData();

    void SetChunk(Chunk* chunk);
    void Reset();

    // Mesh Vertices
    std::vector<uint32_t> vertices_buffer_;
    std::vector<uint32_t> transparent_vertices_buffer_;
    std::vector<uint32_t> solid_fluid_vertices_buffer_;
    std::vector<uint32_t> transparent_fluid_vertices_buffer_;

    size_t transparent_face_count_ = 0;
    size_t solid_face_count_ = 0;
    size_t solid_fluid_face_count_ = 0;
    size_t transparent_fluid_face_count_ = 0;

    size_t greedy_time_;
    size_t cache_time_;

    // Generate the Mesh
    void GenerateMesh();

   private:
    // Used to generate the cache
    void GenerateCache();

    // Generates all the faces and puts them in the cache
    void GenerateFaceCollection();

    // Fluid mesh
    void GenerateFluidMesh();

    // Check if the player can see the mesh
    bool IsFaceVisible(const Cuboid& cube, const BlockPos& pos, uint8_t side);

    bool CompareBlockSide(const BlockPos& pos, uint8_t side, BlockID b);

    // Add faces to the mesh
    void AddModelToMesh(const BlockModel& curr_model,
                        const BlockModel& back_model, glm::ivec3 pos,
                        int u_length, int v_length, bool curr_blank,
                        bool back_blank, int axis);
    void AddFaceToMesh(const BlockFace& face, uint8_t axis, glm::ivec3 from,
                       glm::ivec3 to, bool allow_ao, const BlockPos& pos);

    const BlockID& GetCachedBlockID(const BlockPos& pos) const;
    void SetCachedBlockID(BlockID b, const BlockPos& pos);

    glm::u8vec4 GetAO(uint8_t direction, const BlockPos& pos);

    static constexpr uint64_t kBufferStepSize = 262144;

    Chunk* chunk_;

    // TODO: 3 x is to use cache locality for processing for each axis (haven't
    // implemented yet)
    std::array<BlockID, (kChunkDim + 2) * (kChunkDim + 2) * (kChunkDim + 2) * 3>
        chunk_cache_;
    std::bitset<(kChunkDim + 2) * (kChunkDim + 2) * (kChunkDim + 2)> is_fluid_;
};
}  // namespace Mesh