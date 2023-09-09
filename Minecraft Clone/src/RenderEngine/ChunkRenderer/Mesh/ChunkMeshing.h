#pragma once
#include <vector>

#include "../../../World/Chunk/Chunk.h"

#define L_NN 0b00
#define L_NP 0b01
#define L_PN 0b10
#define L_PP 0b11


namespace Meshing
{

	struct Quad {
		//Position in the slice 
		uint8_t x = 0, y = 0;
		//Size
		uint8_t w = 0, h = 0;

		//Lighting; stored as 16 bit to save mem
		uint16_t Lighting = 0xFFFF;
		uint16_t Texture = 0;
		inline char getLight(uint8_t Location) {
			return 0b1111 & (Lighting >> (Location * 4));
		}

		inline void setLight(uint8_t Location, uint8_t Val) {
			Lighting = Lighting & (~(0b1111 << (Location * 4))); // Clears light value for that location
			Lighting |= (Val << (Location * 4)); //Inserts light value
		}

		//int L_NN = 15, L_NP = 15, L_PP = 15, L_PN = 15; // Lighting : P = Positive; N = Negative

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

		ChunkMeshData(Chunk& chunk) {
			GenerateMesh(chunk);
		}

		//Mesh Vertices
		std::vector<unsigned int> SolidVertices;
		std::vector<unsigned int> TransparentVertices;

		//Position of the mesh in the world
		glm::ivec3 Position = glm::ivec3(0,0,0);
		
		//Generate the Mesh
		void GenerateMesh(Chunk& chunk);

		//Allows you to edit the chunk without remeshing the entire chunk
		void EditBlock(Block block, int x, int y, int z);

	private:

		//Simplifies the Mesh and output it in the vertices array
		void SimplifyMesh();

		//Compare quad sizes and position
		bool compareQuads(Quad q0, Quad q1);

		//Generates all the faces and puts them in the cache
		void GenerateFaceCollection(Chunk& chunk);

		//Ambient Occulsion
		void GenerateAmbientOcculsion(Chunk& chunk);

		//Check if the player can see the mesh
		bool IsFaceVisible(Chunk& chunk, int x, int y, int z, int side);

		//Get face from the cache
		Quad GetFace(int x, int y, int z, int side);

		//Sets the faces in FaceCollectionCache
		void SetFace(int x, int y, int z, int side, Quad quad);

		//Gets block texture 
		int GetTexture(Chunk& chunk, int x, int y, int z, int side);

		//Face data
		Quad* FaceCollectionCache;

		//Add faces to the mesh
		void AddFacetoMesh(Quad quad, int slice, int axis, int face);


		void AddFacetoMesh_X(Quad quad, int slice, int face);
		void AddFacetoMesh_Y(Quad quad, int slice, int face);
		void AddFacetoMesh_Z(Quad quad, int slice, int face);

		//To check if a block had been used in the Greedy Meshing Algorithm

		Quad NullQuad;

	};

	
}

