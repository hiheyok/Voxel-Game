#include "ChunkMeshing.h"

#include "../../../World/Chunk/Block/Blocks.h"
#include <unordered_set>

using namespace Meshing;
using namespace std;
using namespace glm;

void ChunkMeshData::GenerateMesh(Chunk& chunk) {
	//Initialize

	NullQuad.h = 9999;
	NullQuad.w = 9999;
	NullQuad.x = 9999;
	NullQuad.y = 9999;

	NullQuad.Texture = 9999;

	FaceCollectionCache.resize(4096 * 6);

	for (int i = 0; i < 4096 * 6; i++) {
		FaceCollectionCache[i] = NullQuad;
	}

	Position = chunk.Position;

	//Generates the unsimplified mesh first

	GenerateFaceCollection(chunk); //Generates face with their respected textures

	GenerateFaceLighting(chunk); //Add lighting to those faces

	SimplifyMesh(chunk); //Simplifies mesh

	//Clear cache
	FaceCollectionCache.clear();

}

//Checks if there are anything different between q0 and q1
bool ChunkMeshData::compareQuads(Quad q0, Quad q1) {
	if (q0.L_NN != q1.L_NN)
		return false;
	if (q0.L_PN != q1.L_PN)
		return false;
	if (q0.L_PP != q1.L_PP)
		return false;
	if (q0.L_NP != q1.L_NP)
		return false;
	if (q0.Texture != q1.Texture)
		return false;
	return true;
}

//Loops through all the blocks in the chunk and check if each block side is visible. If a block side is visible, it generates the quad and puts it in the cache

void ChunkMeshData::GenerateFaceCollection(Chunk& chunk) {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				if (chunk.GetBlock(x, y, z) != AIR) {
					for (int side = 0; side < 6; side++) {
						if (IsFaceVisible(chunk, x, y, z, side)) {
							Quad quad;
							quad.Texture = GetTexture(chunk, x, y, z, side);
							SetFace(x, y, z, side, quad);
						}
					}
				}
			}
		}
	}
}



//Simplifies mesh (Algorithm: Greedy Meshing): Similar to this https://gist.github.com/Vercidium/a3002bd083cce2bc854c9ff8f0118d33
void ChunkMeshData::SimplifyMesh(Chunk& chunk) {
	//iterate through the x, y, and z axis

	int x[3]{};
	int q[3]{};

	for (int axis = 2; axis < 3; axis++) {

		int Axis0 = (axis + 2) % 3;
		int Axis1 = (axis + 1) % 3;
		int Axis2 = axis;

		//iterate the back and front facing sides
		for (int facing = 0; facing < 2; facing++) {
			//iterate through each slice of a chunk 

			Quad LastQuad = NullQuad;

			for (x[Axis2] = 0; x[Axis2] < 16; x[Axis2]++) {
				for (x[Axis1] = 0; x[Axis1] < 16; x[Axis1]++) {
					for (x[Axis0] = 0; x[Axis0] < 16; x[Axis0]++) {
						if (!compareQuads(GetFace(x[0], x[1], x[2], axis * 2 + facing), NullQuad)) { // Check if quad actually exist first
							LastQuad = GetFace(x[0], x[1], x[2], axis * 2 + facing); //Load in quad face

							int Axis1W = 1;
							int Axis0H = 1;
							int Axis1K = 1;

							for (Axis1W = 1; Axis1W + x[Axis1] <= 16; Axis1W++) { //Go as far as it can until it hit a quad that is different
								q[Axis0] = x[Axis0];
								q[Axis1] = x[Axis1] + Axis1W;
								q[Axis2] = x[Axis2];

								bool IsSame = compareQuads(GetFace(q[0], q[1], q[2], axis * 2 + facing), LastQuad);
								
								if (!IsSame)
									break;
							}

							bool done = false;
							
							//This starts at 1 because we already know that at Axis0H = 0, there's a valid quad

							for (Axis0H = 1; Axis0H + x[Axis0] < 16; Axis0H++) {

								q[Axis0] = x[Axis0] + Axis0H;
								q[Axis1] = x[Axis1];
								q[Axis2] = x[Axis2];

								if (!compareQuads(GetFace(q[0], q[1], q[2], axis * 2 + facing), LastQuad)) { //Checks if it can expand in the other direction first
									Axis1K = Axis1W;
									break;
								}

								//This starts at 1 because we already know it can expand in the other direction
								for (Axis1K = 1; Axis1K < Axis1W; Axis1K++) { //loops through until it finds a break

									q[Axis0] = x[Axis0] + Axis0H;
									q[Axis1] = x[Axis1] + Axis1K;
									q[Axis2] = x[Axis2];
									
									if (!compareQuads(GetFace(q[0], q[1], q[2], axis * 2 + facing), LastQuad)) {
										done = true;
										break;
									}
								}
								if (done) {
									break;
								}
							}

							//Set all of the used quads as NullQuads to prevent them from being used again;
							for (int Axis0H_TMP = 0; Axis0H_TMP < Axis0H; Axis0H_TMP++) {
								for (int Axis1K_TMP = 0; Axis1K_TMP < Axis1K; Axis1K_TMP++) {
									q[Axis0] = x[Axis0] + Axis0H_TMP;
									q[Axis1] = x[Axis1] + Axis1K_TMP;
									q[Axis2] = x[Axis2];
									SetFace(q[0], q[1], q[2], axis * 2 + facing, NullQuad);
								}
							}
							Quad finalq = LastQuad;

							finalq.y = x[Axis0];
							finalq.x = x[Axis1];

							finalq.h = Axis0H;
							finalq.w = Axis1W;

							AddFacetoMesh(finalq, x[Axis2], axis, facing);

							LastQuad = NullQuad;

						}
					}
				}
			}
		}
	}
}

const int xDataBitOffset = 0;
const int yDataBitOffset = 5;
const int zDataBitOffset = 10;
const int blockShadingBitOffset = 15;
const int textureBitOffset = 20;

void ChunkMeshData::AddFacetoMesh(Quad quad, int slice, int axis, int face) {

	unsigned int v[6]{};

	int ParallelAxis = axis;
	int AxisU = ((axis + 1) % 3);
	int AxisV = ((axis + 2) % 3);

	// @ axis = 2

//	AxisU = ((axis + 1 * (axis % 2) + (int)(0.5 * axis + 1) * ((axis + 1) % 2)) % 3); // 1
//	AxisV = ((axis + 2 * (axis % 2) + (int)(-0.5 * axis + 2) * ((axis + 1) % 2)) % 3); // 0

	unsigned int P0[3]{};
	unsigned int P1[3]{};

	P0[ParallelAxis] = (slice - face + 1) << (ParallelAxis * 5);
	P0[AxisU] = (quad.x) << (AxisU * 5);
	P0[AxisV] = (quad.y) << (AxisV * 5);

	P1[ParallelAxis] = (slice - face + 1) << (ParallelAxis * 5);
	P1[AxisU] = (quad.x + quad.w) << (AxisU * 5);
	P1[AxisV] = (quad.y + quad.h) << (AxisV * 5);

	int sy = quad.w << 10;
	int sx = quad.h << 0;

	int NN = quad.L_NN;
	int NP = quad.L_NP;
	int PN = quad.L_PN;
	int PP = quad.L_PP;

	int tex = quad.Texture;

	if (face == 0) {
		SolidVertices.push_back(0u | P0[ParallelAxis] | P1[AxisU] | P0[AxisV] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | 0 | 0 | (tex << textureBitOffset));
		SolidVertices.push_back(0u | P0[ParallelAxis] | P1[AxisU] | P1[AxisV] | (PP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | 0 | (tex << textureBitOffset));
		SolidVertices.push_back(0u | P0[ParallelAxis] | P0[AxisU] | P1[AxisV] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy | (tex << textureBitOffset));
		SolidVertices.push_back(0u | P0[ParallelAxis] | P1[AxisU] | P0[AxisV] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | 0 | 0 | (tex << textureBitOffset));
		SolidVertices.push_back(0u | P0[ParallelAxis] | P0[AxisU] | P1[AxisV] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy | (tex << textureBitOffset));
		SolidVertices.push_back(0u | P0[ParallelAxis] | P0[AxisU] | P0[AxisV] | (NN << blockShadingBitOffset));
		SolidVertices.push_back(0u | 0 | sy | (tex << textureBitOffset));
	}
	else {
		SolidVertices.push_back(0u | P0[ParallelAxis] | P1[AxisU] | P0[AxisV] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | 0 | 0 | (tex << textureBitOffset));
		SolidVertices.push_back(0u | P0[ParallelAxis] | P0[AxisU] | P1[AxisV] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy | (tex << textureBitOffset));
		SolidVertices.push_back(0u | P0[ParallelAxis] | P1[AxisU] | P1[AxisV] | (PP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | 0 | (tex << textureBitOffset));
		SolidVertices.push_back(0u | P0[ParallelAxis] | P1[AxisU] | P0[AxisV] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | 0 | 0 | (tex << textureBitOffset));
		SolidVertices.push_back(0u | P0[ParallelAxis] | P0[AxisU] | P0[AxisV] | (NN << blockShadingBitOffset));
		SolidVertices.push_back(0u | 0 | sy | (tex << textureBitOffset));
		SolidVertices.push_back(0u | P0[ParallelAxis] | P0[AxisU] | P1[AxisV] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy | (tex << textureBitOffset));
		
	}
}

//This generates the lighting by looping through all the blocks in the chunk. If it finds a block that is solid, it will check if there's another block near it that forms a corner, and it makes that spot just a little darker

void ChunkMeshData::GenerateFaceLighting(Chunk& chunk) {
	for (int x = 0 - 1; x < 16 + 1; x++) {
		for (int y = 0 - 1; y < 16 + 1; y++) {
			for (int z = 0 - 1; z < 16 + 1; z++) {

				if (chunk.GetBlock(x, y, z) != AIR) {
					Quad PositiveX = GetFace(x, y, z, PX);
					Quad PositiveY = GetFace(x, y, z, PY);
					Quad PositiveZ = GetFace(x, y, z, PZ);

					Quad NegativeX = GetFace(x, y, z, NX);
					Quad NegativeY = GetFace(x, y, z, NY);
					Quad NegativeZ = GetFace(x, y, z, NZ);

					if (chunk.GetBlock(x + 1, y + 1, z) != AIR) {
						PositiveX.L_NP = 10;
						PositiveX.L_PP = 10;
						PositiveY.L_PN = 10;
						PositiveY.L_PP = 10;
						if (chunk.GetBlock(x + 1, y + 1, z + 1) != AIR) {
							PositiveX.L_NP = 10;
							PositiveY.L_PP = 10;
							PositiveZ.L_PP = 10;
						}
						if (chunk.GetBlock(x + 1, y + 1, z - 1) != AIR) {
							PositiveX.L_NP = 10;
							PositiveY.L_PN = 10;
							NegativeZ.L_PP = 10;
						}
					}
					if (chunk.GetBlock(x - 1, y + 1, z) != AIR) {
						NegativeX.L_NP = 10;
						NegativeX.L_PP = 10;
						PositiveY.L_NN = 10;
						PositiveY.L_NP = 10;
						if (chunk.GetBlock(x - 1, y + 1, z + 1) != AIR) {
							NegativeX.L_NP = 10;
							PositiveY.L_NP = 10;
							PositiveZ.L_NP = 10;
						}
						if (chunk.GetBlock(x - 1, y + 1, z - 1) != AIR) {
							NegativeX.L_NP = 10;
							PositiveY.L_NN = 10;
							NegativeZ.L_NP = 10;
						}
					}
					if (chunk.GetBlock(x, y + 1, z + 1) != AIR) {
						PositiveZ.L_NP = 10;
						PositiveZ.L_PP = 10;
						PositiveY.L_NP = 10;
						PositiveY.L_PP = 10;
						if (chunk.GetBlock(x + 1, y + 1, z + 1) != AIR) {
							PositiveX.L_NP = 10;
							PositiveY.L_PP = 10;
							PositiveZ.L_PP = 10;
						}
						if (chunk.GetBlock(x - 1, y + 1, z + 1) != AIR) {
							NegativeX.L_NP = 10;
							PositiveY.L_NP = 10;
							PositiveZ.L_NP = 10;
						}
					}
					if (chunk.GetBlock(x, y + 1, z - 1) != AIR) {
						NegativeZ.L_NP = 10;
						NegativeZ.L_PP = 10;
						PositiveY.L_NN = 10;
						PositiveY.L_PN = 10;
						if (chunk.GetBlock(x + 1, y + 1, z - 1) != AIR) {
							PositiveX.L_NP = 10;
							PositiveY.L_PN = 10;
							NegativeZ.L_PP = 10;
						}
						if (chunk.GetBlock(x - 1, y + 1, z - 1) != AIR) {
							NegativeX.L_NP = 10;
							PositiveY.L_NN = 10;
							NegativeZ.L_NP = 10;
						}
					}

					if (chunk.GetBlock(x + 1, y - 1, z) != AIR) {
						PositiveX.L_NN = 10;
						PositiveX.L_PN = 10;
						NegativeY.L_PN = 10;
						NegativeY.L_PP = 10;
						if (chunk.GetBlock(x + 1, y - 1, z + 1) != AIR) {
							PositiveX.L_NN = 10;
							NegativeY.L_PP = 10;
							PositiveZ.L_PN = 10;
						}
						if (chunk.GetBlock(x + 1, y - 1, z - 1) != AIR) {
							PositiveX.L_NN = 10;
							NegativeY.L_PN = 10;
							NegativeZ.L_PN = 10;
						}
					}
					if (chunk.GetBlock(x - 1, y - 1, z) != AIR) {
						NegativeX.L_NN = 10;
						NegativeX.L_PN = 10;
						NegativeY.L_NN = 10;
						NegativeY.L_NP = 10;
						if (chunk.GetBlock(x - 1, y - 1, z + 1) != AIR) {
							NegativeX.L_NN = 10;
							NegativeY.L_NP = 10;
							PositiveZ.L_NN = 10;
						}
						if (chunk.GetBlock(x - 1, y - 1, z - 1) != AIR) {
							NegativeX.L_NN = 10;
							NegativeY.L_NN = 10;
							NegativeZ.L_NN = 10;
						}
					}
					if (chunk.GetBlock(x, y - 1, z + 1) != AIR) {
						PositiveZ.L_NN = 10;
						PositiveZ.L_PN = 10;
						NegativeY.L_NP = 10;
						NegativeY.L_PP = 10;
						if (chunk.GetBlock(x + 1, y - 1, z + 1) != AIR) {
							PositiveX.L_NN = 10;
							NegativeY.L_PP = 10;
							PositiveZ.L_PN = 10;
						}
						if (chunk.GetBlock(x - 1, y - 1, z + 1) != AIR) {
							NegativeX.L_NN = 10;
							NegativeY.L_NP = 10;
							PositiveZ.L_NN = 10;
						}
					}
					if (chunk.GetBlock(x, y - 1, z - 1) != AIR) {
						NegativeZ.L_NN = 10;
						NegativeZ.L_PN = 10;
						NegativeY.L_NN = 10;
						NegativeY.L_PN = 10;
						if (chunk.GetBlock(x - 1, y - 1, z - 1) != AIR) {
							NegativeX.L_NN = 10;
							NegativeY.L_NN = 10;
							NegativeZ.L_NN = 10;
						}
						if (chunk.GetBlock(x + 1, y - 1, z - 1) != AIR) {
							PositiveX.L_NN = 10;
							NegativeY.L_PN = 10;
							NegativeZ.L_PN = 10;
						}
					}
				}

			}
		}
	}
}

//Checks if a block side is visible to the player
bool ChunkMeshData::IsFaceVisible(Chunk& chunk, int x, int y, int z, int side) {
	switch (side)
	{
	case NX:
		return chunk.GetBlock(x - 1, y, z) == AIR;
	case PX:
		return chunk.GetBlock(x + 1, y, z) == AIR;
	case NY:
		return chunk.GetBlock(x, y - 1, z) == AIR;
	case PY:
		return chunk.GetBlock(x, y + 1, z) == AIR;
	case NZ:
		return chunk.GetBlock(x, y, z - 1) == AIR;
	case PZ:
		return chunk.GetBlock(x, y, z + 1) == AIR;
	default:
		return false;
	}
}

Quad ChunkMeshData::GetFace(int x, int y, int z, int side) {
	if (x >= 16 || y >= 16 || z >= 16 || x < 0 || y < 0 || z < 0) {
		return NullQuad;
	}
	return FaceCollectionCache.data()[(x * 256 + y * 16 + z) * 6 + side];
}

void ChunkMeshData::SetFace(int x, int y, int z, int side, Quad quad) {
	if (x >= 16 || y >= 16 || z >= 16 || x < 0 || y < 0 || z < 0) {
		return;
	}
	FaceCollectionCache.data()[(x * 256 + y * 16 + z) * 6 + side] = quad;
}

int ChunkMeshData::GetTexture(Chunk& chunk, int x, int y, int z, int side) {
	return BlockList[chunk.GetBlock(x, y, z)]->Texture->GetFace(side);
}