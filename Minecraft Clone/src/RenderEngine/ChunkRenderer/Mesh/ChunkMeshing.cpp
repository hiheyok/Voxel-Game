#include "ChunkMeshing.h"

#include "../../../World/Chunk/Block/Blocks.h"
#include <unordered_set>

using namespace Meshing;
using namespace std;
using namespace glm;

const int blockShadingBitOffset = 15;
const int textureBitOffset = 20;

void ChunkMeshData::GenerateMesh(Chunk& chunk) {
	//Initialize

	NullQuad.h = 255;
	NullQuad.w = 255;
	NullQuad.x = 255;
	NullQuad.y = 255;

	NullQuad.Texture = 255;

	Quad cache[4096 * 6]{};

	FaceCollectionCache = cache;

	for (int i = 0; i < 4096 * 6; i++) {
		FaceCollectionCache[i] = NullQuad;
	}

	Position = chunk.Position;

	//Generates the unsimplified mesh first

	GenerateFaceCollection(chunk); //Generates face with their respected textures

	GenerateAmbientOcculsion(chunk); //AO

	SimplifyMesh(); //Simplifies mesh (Greedy Meshing)

}

//Checks if there are anything different between q0 and q1
bool ChunkMeshData::compareQuads(Quad q0, Quad q1) {
	return (q0.Lighting == q1.Lighting) && (q0.Texture == q1.Texture);
}

//Loops through all the blocks in the chunk and check if each block side is visible. If a block side is visible, it generates the quad and puts it in the cache

void ChunkMeshData::GenerateFaceCollection(Chunk& chunk) {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				if (chunk.GetBlock(x, y, z) == AIR)
					continue;

				for (int side = 0; side < 6; side++) {
					if (!IsFaceVisible(chunk, x, y, z, side))
						continue;

					Quad quad;
					quad.Texture = GetTexture(chunk, x, y, z, side);
					SetFace(x, y, z, side, quad);
				}
			}
		}
	}
}

//Simplifies mesh (Algorithm: Greedy Meshing): Similar to this https://gist.github.com/Vercidium/a3002bd083cce2bc854c9ff8f0118d33
void ChunkMeshData::SimplifyMesh() {
	//iterate through the x, y, and z axis
	int x[3]{};
	int q[3]{};

	for (int axis = 0; axis < 3; axis++) {

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

							for (Axis0H = 0; Axis0H + x[Axis0] < 16; Axis0H++) {

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




void ChunkMeshData::AddFacetoMesh(Quad quad, int slice, int axis, int face) {

	if (axis == 0) { //0 = x axis
		AddFacetoMesh_X(quad, slice, face);
		return;
	}
	if (axis == 1) { //1 = y axis
		AddFacetoMesh_Y(quad, slice, face);
		return;
	}
	if (axis == 2) { //2 = z axis
		AddFacetoMesh_Z(quad, slice, face);
		return;
	}
}

void ChunkMeshData::AddFacetoMesh_X(Quad quad, int slice, int face) {
	uint32_t P0[3]{};
	uint32_t P1[3]{};

	int ParallelAxis = 0;
	int AxisU = 1;
	int AxisV = 2;

	P0[0] = (slice - face + 1) << (ParallelAxis * 5);
	P0[1] = (quad.x) << (AxisU * 5);
	P0[2] = (quad.y) << (AxisV * 5);

	P1[0] = (slice - face + 1) << (ParallelAxis * 5); // x
	P1[1] = (quad.x + quad.w) << (AxisU * 5); // y
	P1[2] = (quad.y + quad.h) << (AxisV * 5); // z

	char NN = quad.getLight(L_NN);
	char NP = quad.getLight(L_NP);
	char PN = quad.getLight(L_PN);
	char PP = quad.getLight(L_PP);

	int sy = quad.w << 10;
	int sx = quad.h << 0;

	int sy0 = 0;
	int sx0 = 0;

	if ((int)NN + (int)PP > (int)NP + (int)PN) {
		int PTMP[3]{ P0[0], P0[1], P0[2] };

		P0[1] = P1[1];

		P1[1] = PTMP[1];

		char NP_TMP = NP;
		NP = PP;
		PP = NP_TMP;

		char NN_TMP = NN;
		NN = PN;
		PN = NN_TMP;

		face = !face;

		int sy_tmp = sy;
		sy = 0;
		sy0 = sy_tmp;
	}

	

	int tex = quad.Texture << textureBitOffset;

	switch (face) {
	case 1:
		SolidVertices.push_back(0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy | tex);
		break;
	case 0:
		SolidVertices.push_back(0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy | tex);
		break;
	}
}

void ChunkMeshData::AddFacetoMesh_Y(Quad quad, int slice, int face) {
	uint32_t P0[3]{};
	uint32_t P1[3]{};

	int ParallelAxis = 1;
	int AxisU = 2;
	int AxisV = 0;

	P0[0] = (slice - face + 1) << (ParallelAxis * 5);
	P0[1] = (quad.y) << (AxisV * 5);
	P0[2] = (quad.x) << (AxisU * 5);


	P1[0] = (slice - face + 1) << (ParallelAxis * 5); // y
	P1[1] = (quad.y + quad.h) << (AxisV * 5); // x
	P1[2] = (quad.x + quad.w) << (AxisU * 5); // z


	char NN = quad.getLight(L_NN);
	char NP = quad.getLight(L_NP);
	char PN = quad.getLight(L_PN);
	char PP = quad.getLight(L_PP);

	int sx = quad.h << 0;
	int sy = quad.w << 10;

	int sy0 = 0;
	int sx0 = 0;

	if ((int)NN + (int)PP > (int)NP + (int)PN) {
		int PTMP[3]{ P0[0], P0[1], P0[2] };

		P0[1] = P1[1];

		P1[1] = PTMP[1];

		char NP_TMP = NP;
		NP = NN;
		NN = NP_TMP;

		char PP_TMP = PP;
		PP = PN;
		PN = PP_TMP;

		face = !face;

		int sx_tmp = sx;
		sx = 0;
		sx0 = sx_tmp;
	}

	

	int tex = quad.Texture << textureBitOffset; //x : z

	switch (face) {
	case 1:
		SolidVertices.push_back(0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)); //0
		SolidVertices.push_back(0u | sx0 | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset)); //1
		SolidVertices.push_back(0u | sx0 | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset)); //2
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset)); //3
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset)); //4
		SolidVertices.push_back(0u | sx0 | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)); //5
		SolidVertices.push_back(0u | sx | sy | tex);
		
		break;
	case 0:

		SolidVertices.push_back(0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy | tex);
		

		break;
	}
}

void ChunkMeshData::AddFacetoMesh_Z(Quad quad, int slice, int face) { //x : x
	uint32_t P0[3]{};
	uint32_t P1[3]{};

	int ParallelAxis = 2;
	int AxisU = 0;
	int AxisV = 1;

	P0[0] = (slice - face + 1) << (ParallelAxis * 5);
	P0[1] = (quad.x) << (AxisU * 5);
	P0[2] = (quad.y) << (AxisV * 5);

	P1[0] = (slice - face + 1) << (ParallelAxis * 5); // z
	P1[1] = (quad.x + quad.w) << (AxisU * 5); // x
	P1[2] = (quad.y + quad.h) << (AxisV * 5); // y

	char NN = quad.getLight(L_NN);
	char NP = quad.getLight(L_NP);
	char PN = quad.getLight(L_PN);
	char PP = quad.getLight(L_PP);

	int sy = quad.h << 10;
	int sx = quad.w << 0;

	int sy0 = 0;
	int sx0 = 0;

	if ((int)NN + (int)PP > (int)NP + (int)PN) {
		int PTMP[3]{ P0[0], P0[1], P0[2] };

		P0[1] = P1[1];

		P1[1] = PTMP[1];

		char NP_TMP = NP;
		NP = PP;
		PP = NP_TMP;

		char NN_TMP = NN;
		NN = PN;
		PN = NN_TMP;

		int sx_tmp = sx;
		sx = 0;
		sx0 = sx_tmp;

		face = !face;
	}

	

	int tex = quad.Texture << textureBitOffset;

	switch (face) {
	case 1:
		SolidVertices.push_back(0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy | tex);
		break;
	case 0:
		SolidVertices.push_back(0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy0 | tex);
		SolidVertices.push_back(0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx0 | sy | tex);
		SolidVertices.push_back(0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset));
		SolidVertices.push_back(0u | sx | sy | tex);
		break;
	}
}

//Creates ambient occulsion 

void ChunkMeshData::GenerateAmbientOcculsion(Chunk& chunk) {

	uint8_t lightlvl = 11;

	for (int axis = 0; axis < 3; axis++) {
		for (int face = 0; face < 2; face++) {
			int p[3]{ 0,0,0 };

			int axis0 = axis;
			int axis1 = (axis + 1) % 3; //u
			int axis2 = (axis + 2) % 3; //v

			int offset = 1 - (2 * face); // main axis offset

			for (p[axis0] = -1; p[axis0] < 17; p[axis0]++) {
				for (p[axis1] = -1; p[axis1] < 17; p[axis1]++) {
					for (p[axis2] = -1; p[axis2] < 17; p[axis2]++) {

						Quad q = GetFace(p[0], p[1], p[2], axis * 2 + face);

						if (compareQuads(NullQuad, q)) //Checks if face doesnt exist
							continue;

						int check[3]{ p[0],p[1],p[2] };

						check[axis0] += offset;

						for (int u = -1; u < 2; u++) {

							for (int v = -1; v < 2; v++) {

								if (v == 0 && u == 0)
									continue;

								check[axis1] = u + p[axis1];
								check[axis2] = v + p[axis2];

								if (chunk.GetBlock(check[0], check[1], check[2]) == AIR)
									continue;

								uint8_t CornerIndex = 0;

								if ((u != 0) && (v != 0)) { //Corners

									CornerIndex |= 0b10 * (u == 1);
									CornerIndex |= 0b01 * (v == 1);

									q.setLight(CornerIndex, lightlvl);

									continue;
								}

								if (u != 0) {

									CornerIndex |= 0b10 * (u == 1);

									q.setLight(CornerIndex, lightlvl);
									q.setLight(CornerIndex | 0b01, lightlvl);

									continue;
								}

								if (v != 0) {

									CornerIndex |= 0b01 * (v == 1);

									q.setLight(CornerIndex, lightlvl);
									q.setLight(CornerIndex | 0b10, lightlvl);

									continue;
								}
							}
						}
						SetFace(p[0], p[1], p[2], axis * 2 + face, q);
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
	if (x >= 16 || y >= 16 || z >= 16 || x < 0 || y < 0 || z < 0)
		return NullQuad;
	return FaceCollectionCache[(x * 256 + y * 16 + z) * 6 + side];
}

void ChunkMeshData::SetFace(int x, int y, int z, int side, Quad quad) {
	if (x >= 16 || y >= 16 || z >= 16 || x < 0 || y < 0 || z < 0)
		return;
	FaceCollectionCache[(x * 256 + y * 16 + z) * 6 + side] = quad;
}

int ChunkMeshData::GetTexture(Chunk& chunk, int x, int y, int z, int side) {
	return BlockList[chunk.GetBlock(x, y, z)]->Texture->GetFace(side);
}