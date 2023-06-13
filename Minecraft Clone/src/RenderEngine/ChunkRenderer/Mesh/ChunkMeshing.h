#pragma once
#include <vector>

#include "../../../World/Chunk/Chunk.h"

namespace Meshing
{
	struct Quad {
		//Position in the slice 
		int x = 0, y = 0;
		//Size
		int w = 0, h = 0;

		//Lighting
		int L_NN = 15, L_NP = 15, L_PP = 15, L_PN = 15; // Lighting : P = Positive; N = Negative

		/*

		Relative to x-axis
			L_NP = Negative Z , Positive Y
		Relative to y-axis
			L_NP = Negative X , Positive Z
		Relative to z-axis
			L_NP = Negative X , Positive Y
		*/

		//Texture layer
		int Texture = 0;
	};

	class ChunkMeshData {
	public:

		//Mesh Vertices
		std::vector<unsigned int> SolidVertices;
		std::vector<unsigned int> TransparentVertices;

		//Position of the mesh in the world
		glm::ivec3 Position = glm::ivec3(0,0,0);
		
		//Generates the Mesh
		void GenerateMesh(Chunk& chunk);

		//Allows you to edit the chunk without remeshing the entire chunk
		void EditBlock(Block block, int x, int y, int z);

	private:

		//Simplifies the Mesh and output it in the vertices array
		void SimplifyMesh(Chunk& chunk);

		//Compare quad sizes and position
		bool compareQuads(Quad q0, Quad q1);

		//Generates all the faces and puts them in the cache
		void GenerateFaceCollection(Chunk& chunk);

		//Generates the lighting for the mesh
		void GenerateFaceLighting(Chunk& chunk);

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

		//To check if a block had been used in the Greedy Meshing Algorithm

		Quad NullQuad;

	};

	
}

