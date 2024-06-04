#pragma once
#include <vector>

#include "../../../Level/Chunk/Chunk.h"

#define L_NN 0b00
#define L_NP 0b01
#define L_PN 0b10
#define L_PP 0b11


namespace Meshing
{

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

	struct Quad {
		uint32_t Data = 0xFFFFFFFF; //Contain Light and Texture
		//Light: first 16 bit
		//Texture:: last 16 bit

		inline char getLight(uint8_t Location) {
			return 0b1111 & (Data >> (Location << 2));
		}

		inline void setLight(uint8_t Location, uint8_t Val) {
			Data &= (~(0b1111 << (Location << 2))); // Clears light value for that location
			Data |= (Val << (Location << 2)); //Inserts light value
		}

		inline uint16_t getTexture() {
			return Data >> 16;
		}

		inline void setTexture(uint32_t tex) {
			Data = (Data & 0xFFFF) | (tex << 16);
		}

		/*
		Relative to x-axis
			L_NP = Negative Y , Positive Z
		Relative to y-axis
			L_NP = Negative Z , Positive X
		Relative to z-axis
			L_NP = Negative X , Positive Y
		*/

	};

	struct QuadWPos : Quad {
		//Position in the slice 
		int8_t x = 0, y = 0;
		//Size
		int8_t w = 0, h = 0;

		BlockID block;

		/*
		Relative to x-axis
			L_NP = Negative Y , Positive Z
		Relative to y-axis
			L_NP = Negative Z , Positive X
		Relative to z-axis
			L_NP = Negative X , Positive Y
		*/

	};

	class ChunkMeshData {
	public:

		ChunkMeshData() {

		}

		ChunkMeshData(Chunk* chunk) {
			GenerateMesh(chunk);
		}

		//Mesh Vertices
		std::vector<unsigned int> SolidVertices;
		std::vector<unsigned int> TransparentVertices;


		//Position of the mesh in the world
		glm::ivec3 Position = glm::ivec3(0,0,0);
		
		//Generate the Mesh
		void GenerateMesh(Chunk* chunk);

		//Allows you to edit the chunk without remeshing the entire chunk
		void EditBlock(Block block, int x, int y, int z);

		//Debug timing and stuff
		double stage0 = 0.0;
		double stage1 = 0.0;
		double stage2 = 0.0;

		size_t IsFaceVisibleCalls = 0;
		size_t GetFaceCalls = 0;
		size_t SetFaceCalls = 0;
		size_t GetTextureCalls = 0;
		size_t CompareQuadCalls = 0;

	private:

		//Simplifies the Mesh and output it in the vertices array
		void SimplifyMesh(Chunk* chunk);

		//Compare quad sizes and position
		inline bool compareQuads(const Quad& q0, const Quad& q1);

		//Generates all the faces and puts them in the cache
		void GenerateFaceCollection(Chunk* chunk);

		//Ambient Occulsion
		void GenerateAmbientOcculsion(Chunk* chunk);

		//Check if the player can see the mesh
		inline bool IsFaceVisible(Chunk* chunk, int x, int y, int z, uint8_t side);
		inline bool IsFaceVisibleUnsafe(Chunk* chunk, int x, int y, int z, uint8_t side);

		//Get face from the cache
		inline Quad& GetFace(int x, int y, int z, uint8_t side);
		inline Quad& GetFaceUnsafe(int x, int y, int z, uint8_t side);

		//Sets the faces in FaceCollectionCache
		inline void SetFace(int x, int y, int z, uint8_t side, Quad quad);
		inline void SetFaceUnsafe(int x, int y, int z, uint8_t side, Quad quad);

		//Raw
		inline void SetFaceRaw(int x, int y, int z, uint8_t side, uint32_t data);
		inline void SetFaceRawUnsafe(int x, int y, int z, uint8_t side, uint32_t data);

		//Gets block texture 
		inline int GetTexture(Chunk* chunk, int x, int y, int z, uint8_t side);
		inline int GetTextureUnsafe(Chunk* chunk, int x, int y, int z, uint8_t side);

		inline bool CompareBlockSide(Chunk* chunk, int x, int y, int z, uint8_t side, BlockID b);
		inline bool CompareBlockSideUnsafe(Chunk* chunk, int x, int y, int z, uint8_t side, BlockID b);

		inline unsigned TrailingZeros(uint32_t n);
		inline unsigned TrailingZeros(uint16_t n);

		//Face data
		Quad* FaceCollectionCache;
		uint16_t* BitsFaceExistCache;
		uint32_t* BitsSolidBlockCache;
		uint32_t* TextureCache;


		//Add faces to the mesh
		inline void AddFacetoMesh(QuadWPos& quad, int slice, int axis, uint8_t face);

		inline void AddFacetoMesh_X(QuadWPos& quad, int slice, uint8_t face);
		inline void AddFacetoMesh_Y(QuadWPos& quad, int slice, uint8_t face);
		inline void AddFacetoMesh_Z(QuadWPos& quad, int slice, uint8_t face);

		//To check if a block had been used in the Greedy Meshing Algorithm
		Quad NullQuad;

		CompactBooleanData booleanMap;

	};

	
}

