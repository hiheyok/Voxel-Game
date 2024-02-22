#include "ChunkMeshing.h"

#include "../../../World/Chunk/Block/Blocks.h"
#include <unordered_set>

using namespace Meshing;
using namespace std;
using namespace glm;

const int blockShadingBitOffset = 15;
const int textureBitOffset = 20;

void ChunkMeshData::GenerateMesh(Chunk* chunk) {
	//Initialize

	if (chunk == nullptr) {
		return;
	}

	NullQuad.Data = 0xFFFFFFFF;

	Quad cache[4096 * 6]{};

	FaceCollectionCache = cache;

	Position = chunk->Position;

	//Generates the unsimplified mesh first
	
	chunk->Use();
	auto t0 = std::chrono::high_resolution_clock::now();
	GenerateFaceCollection(chunk); //Generates face with their respected textures
	auto t1 = std::chrono::high_resolution_clock::now();
	GenerateAmbientOcculsion(chunk); //AO
	auto t2 = std::chrono::high_resolution_clock::now();
	SimplifyMesh(chunk); //Simplifies mesh (Greedy Meshing)
	auto t3 = std::chrono::high_resolution_clock::now();
	chunk->Unuse();

	stage0 = (double)(t1 - t0).count() / 1000.0;
	stage1 = (double)(t2 - t1).count() / 1000.0;
	stage2 = (double)(t3 - t2).count() / 1000.0;
}

//Checks if there are anything different between q0 and q1
inline bool ChunkMeshData::compareQuads(const Quad& q0, const Quad& q1) {
	return q0.Data == q1.Data;
}

//Loops through all the blocks in the chunk and check if each block side is visible. If a block side is visible, it generates the quad and puts it in the cache

void ChunkMeshData::GenerateFaceCollection(Chunk* chunk) {
	for (int z = 1; z < 15; z++) { //Test along x-faces
		/*if ((chunk->Z_block[z] | chunk->TZ_block[z]) == 0) {
			continue;
		}*/

		for (int y = 1; y < 15; y++) {

			/*if ((chunk->Y_block[y] | chunk->TY_block[y]) == 0) {
				continue;
			}*/

			for (int x = 1; x < 15; x++) {

				BlockID b = 0x00;

				b = chunk->GetBlockUnsafe(x, y, z);

				if (b == Blocks.AIR)
					continue;

				for (uint8_t side = 0; side < 2; side++) {

					if (!IsFaceVisibleUnsafe(chunk, x, y, z, 1 - side) || CompareBlockSideUnsafe(chunk, x, y, z, 1 - side, b))
						continue;

					Quad quad;
					quad.setTexture(GetTextureUnsafe(chunk, x, y, z, 1 - side));
					SetFaceUnsafe(x, y, z, 1 - side, quad);
					if (!Blocks.getBlockType(b)->Properties->transparency) {
						booleanMap.InsertBitPos(x, y, z);
					}
					x += side ;
				}
			}
		}
	}

	for (int x = 1; x < 15; x++) { //Test along y-faces

		/*if ((chunk->X_block[x] | chunk->TX_block[x]) == 0) {
			continue;
		}*/

		for (int z = 1; z < 15; z++) {

			/*if ((chunk->Z_block[z] | chunk->TZ_block[z]) == 0) {
				continue;
			}*/

			for (int y = 1; y < 15; y++) {

				BlockID b = 0x00;

				b = chunk->GetBlockUnsafe(x, y, z);

				if (b == Blocks.AIR)
					continue;

				for (uint8_t side = 0; side < 2; side++) {

					if (!IsFaceVisibleUnsafe(chunk, x, y, z, 3 - side) || CompareBlockSideUnsafe(chunk, x, y, z, 3 - side, b))
						continue;

					Quad quad;
					quad.setTexture(GetTextureUnsafe(chunk, x, y, z, 3 - side));
					SetFaceUnsafe(x, y, z, 3 - side, quad);
					if (!Blocks.getBlockType(b)->Properties->transparency) {
						booleanMap.InsertBitPos(x, y, z);
					}
					y += side;
				}
			}
		}
	}

	for (int y = 1; y < 15; y++) { //Test along z-faces

		/*if ((chunk->Y_block[y] | chunk->TY_block[y]) == 0) {
			continue;
		}*/

		for (int x = 1; x < 15; x++) {

			/*if ((chunk->X_block[y] | chunk->TX_block[y]) == 0) {
				continue;
			}*/

			for (int z = 1; z < 15; z++) {

				BlockID b = 0x00;

				b = chunk->GetBlockUnsafe(x, y, z);

				if (b == Blocks.AIR)
					continue;

				for (uint8_t side = 0; side < 2; side++) {

					if (!IsFaceVisibleUnsafe(chunk, x, y, z, 5 - side) || CompareBlockSideUnsafe(chunk, x, y, z, 5 - side, b))
						continue;

					Quad quad;
					quad.setTexture(GetTextureUnsafe(chunk, x, y, z, 5 - side));
					SetFaceUnsafe(x, y, z, 5 - side, quad);
					if (!Blocks.getBlockType(b)->Properties->transparency) {
						booleanMap.InsertBitPos(x, y, z);
					}
					
					z += side;
				}

			}
		}
	}

	for (int faces = 0; faces < 6; faces++) {
		for (int u = 0; u < 16; u++) {
			for (int v = 0; v < 16; v++) {
				int axis = faces >> 1;

				int p[3]{ 0,0,0 };

				p[axis] = 15 * (faces & 0b1);
				p[(axis + 1) % 3] = u;
				p[(axis + 2) % 3] = v;

				BlockID b = 0x00;

				b = chunk->GetBlockUnsafe(p[0], p[1], p[2]);

				if (b == Blocks.AIR)
					continue;

				for (uint8_t side = 0; side < 6; side++) {
					if (!IsFaceVisible(chunk, p[0], p[1], p[2], side) || CompareBlockSide(chunk, p[0], p[1], p[2], side, b))
						continue;

					Quad quad;
					quad.setTexture(GetTextureUnsafe(chunk, p[0], p[1], p[2], side));
					SetFaceUnsafe(p[0], p[1], p[2], side, quad);
					if (!Blocks.getBlockType(b)->Properties->transparency) {
						booleanMap.InsertBitPos(p[0], p[1], p[2]);
					}
				}

			}
		}

	}
}

//Simplifies mesh (Algorithm: Greedy Meshing): Similar to this https://gist.github.com/Vercidium/a3002bd083cce2bc854c9ff8f0118d33
void ChunkMeshData::SimplifyMesh(Chunk* chunk) {
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

			uint32_t LastQuadData = 0xFFFFFFFF;

			for (x[Axis2] = 0; x[Axis2] < 16; x[Axis2]++) {
				for (x[Axis1] = 0; x[Axis1] < 16; x[Axis1]++) {
					for (x[Axis0] = 0; x[Axis0] < 16; x[Axis0]++) {
						if (GetFaceUnsafe(x[0], x[1], x[2], (axis << 1) + facing).Data != 0xFFFFFFFF) { // Check if quad actually exist first
							LastQuadData = GetFaceUnsafe(x[0], x[1], x[2], (axis << 1) + facing).Data; //Load in quad face

							int Axis1W = 0;
							int Axis0H = 0;
							
							for (Axis1W = x[Axis1]; Axis1W < 16; Axis1W++) { //Go as far as it can until it hit a quad that is different
								q[Axis0] = x[Axis0];
								q[Axis1] = Axis1W;
								q[Axis2] = x[Axis2];

								if (GetFaceUnsafe(q[0], q[1], q[2], (axis << 1) + facing).Data != LastQuadData) {//If isn't same, break
									Axis1W--;
									break;
								}
							}

							if (Axis1W == 16) {
								Axis1W--;
							}

							bool done = false;

							for (Axis0H = x[Axis0]; Axis0H < 16; Axis0H++) {

								q[Axis0] = Axis0H;
								q[Axis1] = x[Axis1];
								q[Axis2] = x[Axis2];

								if (GetFaceUnsafe(q[0], q[1], q[2], (axis << 1) + facing).Data != LastQuadData) { //Checks if it can expand in the other direction first
									Axis0H--;
									break;
								}

								for (int Axis1K = x[Axis1]; Axis1K <= Axis1W; Axis1K++) { //loops through until it finds a break

									q[Axis0] = Axis0H;
									q[Axis1] = Axis1K;
									q[Axis2] = x[Axis2];

									if (GetFaceUnsafe(q[0], q[1], q[2], (axis << 1) + facing).Data != LastQuadData) {
										done = true;
										Axis0H--;
										break;
									}
								}
								if (done) {
									break;
								}
							}

							if (Axis0H == 16) {
								Axis0H--;
							}

							//Set all of the used quads as 0xFFFFFFFF to prevent them from being used again;
							q[Axis2] = x[Axis2];

							for (int Axis0H_TMP = x[Axis0]; Axis0H_TMP <= Axis0H; Axis0H_TMP++) {

								q[Axis0] = Axis0H_TMP;

								for (int Axis1K_TMP = x[Axis1]; Axis1K_TMP <= Axis1W; Axis1K_TMP++) {
									q[Axis1] = Axis1K_TMP;
									SetFaceRawUnsafe(q[0], q[1], q[2], (axis << 1) + facing, 0xFFFFFFFF);
								}
							}

							QuadWPos finalq;

							finalq.y = x[Axis0];
							finalq.x = x[Axis1];

							finalq.h = Axis0H - x[Axis0] + 1;
							finalq.w = Axis1W - x[Axis1] + 1;

							finalq.Data = LastQuadData;

							finalq.block = chunk->GetBlockUnsafe(x[0], x[1], x[2]);

							AddFacetoMesh(finalq, x[Axis2], axis, facing);

							LastQuadData = 0xFFFFFFFF;

							x[Axis0] = Axis0H;
						}
					}
				}
			}
		}
	}
}

inline void ChunkMeshData::AddFacetoMesh(QuadWPos& quad, int slice, int axis, uint8_t face) {

	switch (axis) {
	case 0:
		AddFacetoMesh_X(quad, slice, face);
		return;
	case 1:
		AddFacetoMesh_Y(quad, slice, face);
		return;
	case 2:
		AddFacetoMesh_Z(quad, slice, face);
		return;
	}
}

inline void ChunkMeshData::AddFacetoMesh_X(QuadWPos& quad, int slice, uint8_t face) {
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

	uint32_t tex = (uint32_t)quad.getTexture() << textureBitOffset;

	bool transparency = Blocks.getBlockType(quad.block)->Properties->transparency;

	vector<unsigned int>* out = (transparency ? &TransparentVertices : &SolidVertices);

	switch (face) {
	case 1:
		out->insert(out->end(), {
		0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)
		,0u | sx0 | sy0 | tex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)
		,0u | sx | sy | tex });
		break;
	case 0:
		out->insert(out->end(), {
		0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)
		,0u | sx0 | sy0 | tex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)
		,0u | sx | sy | tex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex
			});
		break;
	}
}

inline void ChunkMeshData::AddFacetoMesh_Y(QuadWPos& quad, int slice, uint8_t face) {
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

	uint32_t tex = (uint32_t)quad.getTexture() << textureBitOffset; //x : z

	bool transparency = Blocks.getBlockType(quad.block)->Properties->transparency;

	vector<unsigned int>* out = (transparency ? &TransparentVertices : &SolidVertices);

	switch (face) {
	case 1:

		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset) //0
		,0u | sx0 | sy0 | tex
		,0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset) //1
		,0u | sx0 | sy | tex
		,0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset) //2
		,0u | sx | sy0 | tex
		,0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset) //3
		,0u | sx | sy0 | tex
		,0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset) //4
		,0u | sx0 | sy | tex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset) //5
		,0u | sx | sy | tex
			});
		break;
	case 0:

		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)
		,0u | sx0 | sy0 | tex
		,0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex
		,0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex
		,0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)
		,0u | sx | sy | tex
		,0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex
			});
		break;
	}
}

inline void ChunkMeshData::AddFacetoMesh_Z(QuadWPos& quad, int slice, uint8_t face) { //x : x
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
		uint32_t PTMP[3]{ P0[0], P0[1], P0[2] };

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

	uint32_t tex = (uint32_t)quad.getTexture() << textureBitOffset;

	bool transparency = Blocks.getBlockType(quad.block)->Properties->transparency;

	vector<unsigned int>* out = (transparency ? &TransparentVertices : &SolidVertices);

	switch (face) {
	case 1:
		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)
		,0u | sx0 | sy0 | tex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx | sy0 | tex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx0 | sy | tex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx0 | sy | tex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx | sy0 | tex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)
		,0u | sx | sy | tex
			});
		break;
	case 0:
		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)
		,0u | sx0 | sy0 | tex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx0 | sy | tex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx | sy0 | tex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx | sy0 | tex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx0 | sy | tex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)
		,0u | sx | sy | tex
			});
		break;
	}
}

//Creates ambient occulsion 

void ChunkMeshData::GenerateAmbientOcculsion(Chunk* chunk) {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {

				if ((!booleanMap.GetBitPos(x, y, z))) {
					continue;
				}

				int pos[3]{ x, y, z };

				for (int side = 0; side < 6; side++) {
					uint8_t axis = side >> 1;
					uint8_t face = side & 0b1;

					uint8_t axis1 = (axis + 1) % 3;
					uint8_t axis2 = (axis + 2) % 3;

					char offset = 1 - (2 * face);

					Quad& q = GetFaceUnsafe(x, y, z, side);

					if (q.Data == 0xFFFFFFFF) //Checks if face doesnt exist
						continue;

					int check[3]{ x, y, z };

					check[axis] += offset;

					for (int u = -1; u <= 1; u++) {

						for (int v = -1; v <= 1; v++) {

							if (v == 0 && u == 0)
								continue;

							check[axis1] = u + pos[axis1];
							check[axis2] = v + pos[axis2];

							if (!Blocks.getBlockType(chunk->GetBlock(check[0], check[1], check[2]))->Properties->isSolid)
								continue;

							uint8_t CornerIndex = 0;

							if ((u != 0) && (v != 0)) { //Corners

								CornerIndex |= 0b10 * (u == 1);
								CornerIndex |= 0b01 * (v == 1);

								q.setLight(CornerIndex, q.getLight(CornerIndex) - 2 * (q.getLight(CornerIndex) > 1));
	

								continue;
							}

							if (u != 0) {

								CornerIndex |= 0b10 * (u == 1);

								q.setLight(CornerIndex, q.getLight(CornerIndex) - 2 * (q.getLight(CornerIndex) > 1));
								q.setLight(CornerIndex | 0b01, q.getLight(CornerIndex | 0b01) - 2 * (q.getLight(CornerIndex | 0b01) > 1));

								continue;
							}

							if (v != 0) {

								CornerIndex |= 0b01 * (v == 1);

								q.setLight(CornerIndex, q.getLight(CornerIndex) - 2 * (q.getLight(CornerIndex) > 1));
								q.setLight(CornerIndex | 0b10, q.getLight(CornerIndex | 0b10) - 2 * (q.getLight(CornerIndex | 0b10) > 1));

								continue;
							}
						}
					}
				}
			}
		}
	}

	
}

//Checks if a block side is visible to the player
inline bool ChunkMeshData::IsFaceVisible(Chunk* chunk, int x, int y, int z, uint8_t side) {
	//IsFaceVisibleCalls++;

	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return Blocks.getBlockType(chunk->GetBlock(p[0], p[1], p[2]))->Properties->transparency;
}

inline bool ChunkMeshData::IsFaceVisibleUnsafe(Chunk* chunk, int x, int y, int z, uint8_t side) {
	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return Blocks.getBlockType(chunk->GetBlockUnsafe(p[0], p[1], p[2]))->Properties->transparency;
}

inline bool ChunkMeshData::CompareBlockSide(Chunk* chunk, int x, int y, int z, uint8_t side, BlockID b) {
	//IsFaceVisibleCalls++;

	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return chunk->GetBlock(p[0], p[1], p[2]) == b;
}

inline bool ChunkMeshData::CompareBlockSideUnsafe(Chunk* chunk, int x, int y, int z, uint8_t side, BlockID b) {
	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return chunk->GetBlockUnsafe(p[0], p[1], p[2]) == b;
}

inline Quad& ChunkMeshData::GetFace(int x, int y, int z, uint8_t side) {
	if (x >= 16 || y >= 16 || z >= 16 || x < 0 || y < 0 || z < 0)
		return NullQuad;
	return GetFaceUnsafe(x, y, z, side);
}

inline Quad& ChunkMeshData::GetFaceUnsafe(int x, int y, int z, uint8_t side) {
	return FaceCollectionCache[((uint16_t)side << 12) + ((x << 8) + (y << 4) + z)];
}

inline void ChunkMeshData::SetFace(int x, int y, int z, uint8_t side, Quad quad) {
	if (x >= 16 || y >= 16 || z >= 16 || x < 0 || y < 0 || z < 0)
		return;
	SetFaceUnsafe(x, y, z, side, quad);
}

inline void ChunkMeshData::SetFaceUnsafe(int x, int y, int z, uint8_t side, Quad quad) {
	FaceCollectionCache[((uint16_t)side << 12) + ((x << 8) + (y << 4) + z)] = quad;
}


inline void ChunkMeshData::SetFaceRaw(int x, int y, int z, uint8_t side, uint32_t data) {
	if (x >= 16 || y >= 16 || z >= 16 || x < 0 || y < 0 || z < 0)
		return;
	SetFaceRawUnsafe(x, y, z, side, data);
}

inline void ChunkMeshData::SetFaceRawUnsafe(int x, int y, int z, uint8_t side, uint32_t data) {
	FaceCollectionCache[((uint16_t)side << 12) + ((x << 8) + (y << 4) + z)].Data = data;
}

inline int ChunkMeshData::GetTexture(Chunk* chunk, int x, int y, int z, uint8_t side) {
	return Blocks.getBlockType(chunk->GetBlock(x, y, z))->Texture->GetFace(side);
}

inline int ChunkMeshData::GetTextureUnsafe(Chunk* chunk, int x, int y, int z, uint8_t side) {
	return Blocks.getBlockType(chunk->GetBlockUnsafe(x, y, z))->Texture->GetFace(side);
}