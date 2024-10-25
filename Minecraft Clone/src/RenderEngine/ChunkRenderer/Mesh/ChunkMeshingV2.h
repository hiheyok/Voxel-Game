#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include  <glm/vec4.hpp>
#include "../../../Level/Chunk/Chunk.h"
#include "../../../Client/Profiler/PerformanceProfiler.h"
#include "../../../Utils/Containers/BitStorage.h"

namespace MeshingV2 {

	static PerformanceProfiler profiler;

	struct CompactBooleanData {

		inline void InsertBit(size_t index) {
			size_t NumIndex = index >> 6;
			size_t Index = index & 0b111111;

			data[NumIndex] |= (0b1LL << Index);

		}

		inline bool Getbit(size_t index) {
			size_t NumIndex = index >> 6;
			size_t Index = index & 0b111111;
			return (data[NumIndex] >> Index) & 0b1;
		}

		inline void InsertBitPos(uint16_t x, uint16_t y, uint16_t z) {
			InsertBit((x << 8) + (y << 4) + z);
		}

		inline bool GetBitPos(uint16_t x, uint16_t y, uint16_t z) {
			return Getbit((x << 8) + (y << 4) + z);
		}

		inline void clear() {
			for (int i = 0; i < 64; i++) {
				data[i] = 0;
			}
		}

		uint64_t data[4 * 16]{};
	};

	struct ChunkVertexData {
		ChunkVertexData() {

		}

		~ChunkVertexData() {
			solidVertices.clear();
			transparentVertices.clear();
		}

		std::vector<uint32_t> solidVertices;
		std::vector<uint32_t> transparentVertices;
		glm::ivec3 Position = glm::ivec3(0, 0, 0);
	};


	class ChunkMeshData {
	public:
		ChunkMeshData() {

		}

		void setChunk(Chunk* pChunk);

		void reset();

		//Mesh Vertices
		std::vector<unsigned int> VerticesBuffer;
		std::vector<unsigned int> TransparentVerticesBuffer;

		uint64_t transparentFaceCount = 0;
		uint64_t solidFaceCount = 0;

		//Position of the mesh in the world
		glm::ivec3 Position = glm::ivec3(0, 0, 0);

		//Generate the Mesh
		void GenerateMesh();

		void AddBlock(int x, int y, int z);

		void GenerateCache();

		//Allows you to edit the chunk without remeshing the entire chunk
		void EditBlock(Block block, int x, int y, int z);

	private:
		

		//Generates all the faces and puts them in the cache
		void GenerateFaceCollection();

		//Check if the player can see the mesh
		inline bool IsFaceVisible(const Cuboid& cube, int x, int y, int z, uint8_t side);
		inline bool IsFaceVisibleUnsafe(const Cuboid& cube, int x, int y, int z, uint8_t side);

		//Gets block texture 
		inline int GetTexture(int x, int y, int z, uint8_t side);
		inline int GetTextureUnsafe(int x, int y, int z, uint8_t side);

		inline bool CompareBlockSide(int x, int y, int z, uint8_t side, BlockID b);
		inline bool CompareBlockSideUnsafe(int x, int y, int z, uint8_t side, BlockID b);

		//Add faces to the mesh
		inline void AddFacetoMesh(const BlockFace& face, uint8_t axis, glm::ivec3 From, glm::ivec3 To, bool allowAO, int x, int y, int z);

		inline void AddFacetoMesh_X(const BlockFace& face, uint8_t direction, glm::ivec3 From, glm::ivec3 To, bool allowAO, int x, int y, int z);
		inline void AddFacetoMesh_Y(const BlockFace& face, uint8_t direction, glm::ivec3 From, glm::ivec3 To, bool allowAO, int x, int y, int z);
		inline void AddFacetoMesh_Z(const BlockFace& face, uint8_t direction, glm::ivec3 From, glm::ivec3 To, bool allowAO, int x, int y, int z);

		inline BlockID& getCachedBlockID(int x, int y, int z);
		inline BlockID& getCachedBlockID(int* pos);
		inline void setCachedBlockID(BlockID b, int x, int y, int z);


		inline glm::ivec4 getAO(uint8_t direction, int x, int y, int z);

		//To check if a block had been used in the Greedy Meshing Algorithm
		CompactBooleanData booleanMap;

		const uint64_t BUFFER_SIZE_STEP = 262144;

		Chunk* chunk;

		BlockID ChunkCache[18 * 18 * 18]{NULL};
	};
}