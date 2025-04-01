#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include "Core/Typenames.h"

class Chunk;

struct Cuboid;
struct BlockFace;

// TODO: refactor this

namespace Mesh {
    struct ChunkVertexData {
        ChunkVertexData() {

        }

        ~ChunkVertexData() {
            solidVertices.clear();
            transparentVertices.clear();
        }

        std::vector<uint32_t> solidVertices;
        std::vector<uint32_t> transparentVertices;
        ChunkPos position_;
        size_t time_;
    };


    class ChunkMeshData {
    public:
        ChunkMeshData() {

        }

        void SetChunk(Chunk* pChunk);

        void Reset();

        //Mesh Vertices
        std::vector<unsigned int> vertices_buffer_;
        std::vector<unsigned int> transparent_vertices_buffer_;

        uint64_t transparent_face_count_ = 0;
        uint64_t solid_face_count_ = 0;

        //Generate the Mesh
        void GenerateMesh();

        void GenerateCache();

    private:
        

        //Generates all the faces and puts them in the cache
        void GenerateFaceCollection();

        //Check if the player can see the mesh
        inline bool IsFaceVisible(const Cuboid& cube, int x, int y, int z, uint8_t side);
        inline bool IsFaceVisibleUnsafe(const Cuboid& cube, int x, int y, int z, uint8_t side);

        inline bool CompareBlockSide(int x, int y, int z, uint8_t side, BlockID b);
        inline bool CompareBlockSideUnsafe(int x, int y, int z, uint8_t side, BlockID b);

        //Add faces to the mesh
        inline void AddFacetoMesh(const BlockFace& face, uint8_t axis_, glm::ivec3 from_, glm::ivec3 to_, bool allowAO, int x, int y, int z);

        inline const BlockID& GetCachedBlockID(int x, int y, int z) const;
        inline const BlockID& GetCachedBlockID(int* pos) const;
        inline void SetCachedBlockID(BlockID b, int x, int y, int z);


        inline glm::u8vec4 getAO(uint8_t direction, int x, int y, int z);

        //To check if a block had been used in the Greedy Meshing Algorithm
        const uint64_t BUFFER_SIZE_STEP = 262144;

        Chunk* chunk_;

        BlockID chunk_cache_[18 * 18 * 18 * 3]{NULL}; // Use multiple copies of chunk in different ordering to optimize cache hits
    };
}