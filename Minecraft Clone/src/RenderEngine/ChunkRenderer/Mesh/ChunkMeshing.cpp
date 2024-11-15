#include "ChunkMeshing.h"
#include <immintrin.h>
#include "../../../Level/Chunk/Block/Blocks.h"
#include <bit> 

using namespace Meshing;
using namespace std;
using namespace glm;

const int blockShadingBitOffset = 15;
const int textureBitOffset = 10;
const int NumTextureBitOffset = 22;

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

	delete[] BitsFaceExistCache;
	delete[] BitsSolidBlockCache;
	delete[] TextureCache;
}

//Checks if there are anything different between q0 and q1
inline bool ChunkMeshData::compareQuads(const Quad& q0, const Quad& q1) {
	return q0.Data == q1.Data;
}


//Loops through all the blocks in the chunk and check if each block side is visible. If a block side is visible, it generates the quad and puts it in the cache

void ChunkMeshData::GenerateFaceCollection(Chunk* chunk) {
	uint32_t CHUNK_SIZE_P = 16 + 2;
	uint32_t CoordOffset = CHUNK_SIZE_P * CHUNK_SIZE_P;
	uint32_t TypeOffset = CoordOffset * 6;

	std::vector<BlockType> BlockTypeCache(Blocks.BlockTypeData.size());

	BitsFaceExistCache = new uint16_t[16 * 16 * 6]{0};
	BitsSolidBlockCache = new uint32_t[18 * 18]{ 0 };
	TextureCache = new uint32_t[BlockTypeCache.size() * 6]{0};

	for (BlockID b = 0; b < BlockTypeCache.size(); b++) {
		Block* block = Blocks.BlockTypeData[b];

		BlockTypeCache[b] = *block->Properties;

		for (int face = 0; face < 6; face++) {
			TextureCache[b * 6 + face] = block->Texture->GetFace(face);
		}

	}
	

	memset(BitsFaceExistCache, 0, 16 * 16 * 6 * sizeof(uint16_t));
	memset(BitsSolidBlockCache, 0, 16 * 16 * sizeof(uint32_t));

	std::vector<uint32_t> Bits(CoordOffset * 6 * 2);

	memset(Bits.data(), 0, CoordOffset * 6);

	//Get main voxels'
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				BlockID b = 0x00;

				b = chunk->GetBlockUnsafe(x, y, z);

				if (b == Blocks.AIR)
					continue;

				int BlockTypeOffset = BlockTypeCache[b].transparency * TypeOffset;

				if (BlockTypeCache[b].isSolid) {
					BitsSolidBlockCache[(x + 1) + (y + 1) * 18] |= 1U << (z + 1);
				}

				int xp = x + 1;
				int yp = y + 1;
				int zp = z + 1;

				Bits[yp + zp * 18 + BlockTypeOffset] |= 1U << xp;
				Bits[zp + xp * 18 + (CoordOffset * 2) + BlockTypeOffset] |= 1U << yp;
				Bits[xp + yp * 18 + (CoordOffset * 4) + BlockTypeOffset] |= 1U << zp;
			}
		}
	}

	int p[3]{ 0,0,0 };

	//Get side voxels
	for (int faces = 0; faces < 6; faces++) {

		int axis = faces >> 1;

		p[axis] = 17 * (faces & 0b1) - 1;

		ChunkContainer* Neighbor = chunk->GetNeighbor(faces);

		if (Neighbor == nullptr) continue;

		for (int u = 0; u < 16; u++) {

			p[(axis + 1) % 3] = u;

			for (int v = 0; v < 16; v++) {
				
				p[(axis + 2) % 3] = v;

				BlockID b = chunk->GetBlock(p[0], p[1], p[2]);

				if (b == Blocks.AIR)
					continue;

				int BlockTypeOffset = BlockTypeCache[b].transparency * TypeOffset;

				if (BlockTypeCache[b].isSolid) {
					BitsSolidBlockCache[(p[0] + 1) + (p[1] + 1) * 18] |= 1U << (p[2] + 1);
				}

				int xp = p[0] + 1;
				int yp = p[1] + 1;
				int zp = p[2] + 1;

				Bits[yp + zp * 18 + BlockTypeOffset] |= 1U << xp;
				Bits[zp + xp * 18 + (CoordOffset << 1) + BlockTypeOffset] |= 1U << yp;
				Bits[xp + yp * 18 + (CoordOffset << 2) + BlockTypeOffset] |= 1U << zp;

			}
		}
	}

	memcpy(Bits.data() + CoordOffset + TypeOffset, Bits.data() + TypeOffset, CoordOffset * sizeof(uint32_t));
	memcpy(Bits.data() + 3 * CoordOffset + TypeOffset, Bits.data() + (CoordOffset << 1) + TypeOffset, CoordOffset * sizeof(uint32_t));
	memcpy(Bits.data() + 5 * CoordOffset + TypeOffset, Bits.data() + (CoordOffset << 2) + TypeOffset, CoordOffset * sizeof(uint32_t));

	memcpy(Bits.data() + CoordOffset, Bits.data(), CoordOffset * sizeof(uint32_t));
	memcpy(Bits.data() + 3 * CoordOffset, Bits.data() + (CoordOffset << 1), CoordOffset * sizeof(uint32_t));
	memcpy(Bits.data() + 5 * CoordOffset, Bits.data() + (CoordOffset << 2), CoordOffset * sizeof(uint32_t));

	//Cull

	__m256i Reg1, Reg2, Reg1T, Reg2T;
	__m256i Bitshift = _mm256_set_epi32(1, 1, 1, 1, 1, 1, 1, 1);
	__m256i all_ones = _mm256_set_epi32(-1, -1, -1, -1, -1, -1, -1, -1);

	//Reg1 -> Solids
	//Reg2 -> Transparent

	for (int face = 0; face < 6; face++) {
		int i = 0;

		for (i = 0; i < (CoordOffset / 8); i++) {

			uint32_t index = i * 8;

			Reg1T = _mm256_loadu_si256((const __m256i*)(Bits.data() + index + face * CoordOffset));//_mm256_loadu_epi32(Bits.data() + index + face * CoordOffset);
			Reg2T = _mm256_loadu_si256((const __m256i*)(Bits.data() + index + face * CoordOffset + TypeOffset));

			Reg1 = _mm256_loadu_si256((const __m256i*)(Bits.data() + index + face * CoordOffset));
			Reg2 = _mm256_loadu_si256((const __m256i*)(Bits.data() + index + face * CoordOffset + TypeOffset));

			if (face & 0b1) {
				Reg1 = _mm256_sllv_epi32(Reg1, Bitshift);
				Reg2 = _mm256_sllv_epi32(Reg2, Bitshift);
			}
			else {
				Reg1 = _mm256_srlv_epi32(Reg1, Bitshift);
				Reg2 = _mm256_srlv_epi32(Reg2, Bitshift);
			}

			Reg1 = _mm256_xor_si256(Reg1, all_ones);
			Reg2 = _mm256_xor_si256(Reg2, all_ones);

			Reg1 = _mm256_and_si256(Reg1, Reg1T);
			Reg2 = _mm256_and_si256(Reg2, Reg2T);

			memcpy(Bits.data() + index + face * CoordOffset, (uint32_t*)(&Reg1), 8 * 4);
			memcpy(Bits.data() + index + face * CoordOffset + TypeOffset, (uint32_t*)(&Reg2), 8 * 4);
		}

		i = i * 8;

		for (; i < CoordOffset; i++) {
			uint32_t index = i;

			uint32_t tmp1 = Bits[index + face * CoordOffset];
			uint32_t tmp2 = Bits[index + face * CoordOffset + TypeOffset];

			if (face & 0b1) {
				tmp1 <<= 1;
				tmp2 <<= 1;
			}
			else {
				tmp1 >>= 1;
				tmp2 >>= 1;
			}


			Bits[index + face * CoordOffset] &= ~tmp1;
			Bits[index + face * CoordOffset + TypeOffset] &= ~tmp2;
		}
	}


	//Fixes transparent and solid block overlap
	for (int direction = 0; direction < 3; direction++) {
		for (int u = 0; u < 18; u++) {
			for (int v = 0; v < 18; v++) {
				int IndexSolid = u + v * 18 + (direction * 2) * CoordOffset;
				int IndexTransparent = u + v * 18 + (direction * 2 + 1) * CoordOffset + TypeOffset;

				uint32_t ColSolids = Bits[IndexSolid] << 1;
				uint32_t ColTrans = Bits[IndexTransparent];

				Bits[IndexTransparent] = (ColSolids ^ ColTrans) & (~ColSolids);
				//Opposite Side
				IndexSolid = u + v * 18 + (direction * 2 + 1) * CoordOffset;
				IndexTransparent = u + v * 18 + (direction * 2) * CoordOffset + TypeOffset;

				ColSolids = Bits[IndexSolid] >> 1;
				ColTrans = Bits[IndexTransparent];

				Bits[IndexTransparent] = (ColSolids ^ ColTrans) & (~ColSolids);

			}
		}
	}

	uint32_t P[3]{ 0 };

	for (uint32_t face = 0; face < 6; face++) {
		for (int u = 1; u < 17; u++) {
			for (int v = 1; v < 17; v++) {
				for (int Types = 0; Types < 2; Types++) {
					uint32_t Index = u + (v * CHUNK_SIZE_P) + (face * CoordOffset);

					uint16_t c = (Bits[Index + TypeOffset * Types] >> 1) & 0xFFFF;
					uint32_t tOffset = 0;

					while (c != 0) {
						int TrailingZeroCount = std::countr_zero((uint32_t)c);
						c = c >> TrailingZeroCount;
						tOffset += TrailingZeroCount;

						uint32_t D = face >> 1;

						P[D] = tOffset;
						P[(D + 1) % 3] = u - 1;
						P[(D + 2) % 3] = v - 1;

						int x = P[0];
						int y = P[1];
						int z = P[2];

						BlockID b = chunk->GetBlockUnsafe(x, y, z);

						uint8_t LightLvl = chunk->Lighting.GetLighting(x,y,z);

						Quad quad;
						quad.setLight(L_NN, LightLvl);
						quad.setLight(L_NP, LightLvl);
						quad.setLight(L_PP, LightLvl);
						quad.setLight(L_PN, LightLvl);
						quad.setTexture(TextureCache[b * 6 + face]);
						SetFaceUnsafe(x, y, z, face, quad);

						BitsFaceExistCache[(u - 1) + (tOffset) * 16 + face * 256] |= 1U << (v - 1);

						if (!BlockTypeCache[b].transparency) {
							booleanMap.InsertBitPos(x, y, z);
						}

						c >>= 1;
						tOffset++;
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
					uint16_t column = BitsFaceExistCache[x[Axis1] + x[Axis2] * 16 + ((axis << 1) + facing) * 256];

					if (column == 0) continue;

					for (x[Axis0] = 0; x[Axis0] < 16; x[Axis0]++) {

						uint32_t TestQuad = GetFaceUnsafe(x[0], x[1], x[2], (axis << 1) + facing).Data;

						if (TestQuad != 0xFFFFFFFF) { // Check if quad actually exist first
							LastQuadData = TestQuad; //Load in quad face

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

	int sy = quad.w << 5;
	int sx = quad.h << 0;

	int sy0 = 0;
	int sx0 = 0;

	uint32_t tex = (uint32_t)quad.getTexture() << textureBitOffset;
	uint32_t NumTex = (uint32_t)Blocks.getBlockType(quad.block)->Texture->GetNumTextures(PX + face) << NumTextureBitOffset;

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

	bool transparency = Blocks.getBlockType(quad.block)->Properties->transparency;

	vector<unsigned int>* out = (transparency ? &TransparentVertices : &SolidVertices);

	switch (face) {
	case 1:
		out->insert(out->end(), {
		0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)
		,0u | sx0 | sy0 | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)
		,0u | sx | sy | tex | NumTex });
		break;
	case 0:
		out->insert(out->end(), {
		0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)
		,0u | sx0 | sy0 | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)
		,0u | sx | sy | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
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
	int sy = quad.w << 5;


	int sy0 = 0;
	int sx0 = 0;

	uint32_t tex = (uint32_t)quad.getTexture() << textureBitOffset;
	uint32_t NumTex = (uint32_t)Blocks.getBlockType(quad.block)->Texture->GetNumTextures(PY + face) << NumTextureBitOffset;

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

	bool transparency = Blocks.getBlockType(quad.block)->Properties->transparency;

	vector<unsigned int>* out = (transparency ? &TransparentVertices : &SolidVertices);

	switch (face) {
	case 1:

		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset) //0
		,0u | sx0 | sy0 | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset) //1
		,0u | sx0 | sy | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset) //2
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset) //3
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset) //4
		,0u | sx0 | sy | tex | NumTex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset) //5
		,0u | sx | sy | tex | NumTex
			});
		break;
	case 0:

		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)
		,0u | sx0 | sy0 | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)
		,0u | sx | sy | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
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

	int sy = quad.h << 5;
	int sx = quad.w << 0;

	int sy0 = 0;
	int sx0 = 0;

	uint32_t tex = (uint32_t)quad.getTexture() << textureBitOffset;
	uint32_t NumTex = (uint32_t)Blocks.getBlockType(quad.block)->Texture->GetNumTextures(PZ + face) << NumTextureBitOffset;

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

	bool transparency = Blocks.getBlockType(quad.block)->Properties->transparency;

	vector<unsigned int>* out = (transparency ? &TransparentVertices : &SolidVertices);

	switch (face) {
	case 1:
		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)
		,0u | sx0 | sy0 | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)
		,0u | sx | sy | tex | NumTex
			});
		break;
	case 0:
		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)
		,0u | sx0 | sy0 | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
		,0u | P0[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)
		,0u | sx | sy | tex | NumTex
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

							bool isSolid = (BitsSolidBlockCache[check[0] + 1 + (check[1] + 1) * 18] >> (check[2] + 1)) & 0b1;

							if (!isSolid)
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
	if ((x | y | z) >> 4)
		return NullQuad;
	return GetFaceUnsafe(x, y, z, side);
}

inline Quad& ChunkMeshData::GetFaceUnsafe(int x, int y, int z, uint8_t side) {
	return FaceCollectionCache[((uint16_t)side << 12) + ((x << 8) + (y << 4) + z)];
}

inline void ChunkMeshData::SetFace(int x, int y, int z, uint8_t side, Quad quad) {
	if ((x | y | z) >> 4)
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