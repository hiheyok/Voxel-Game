#include <vector>
#include <iterator>
#include <chrono>
#include "../../../World/Chunk/Block/Blocks.h"
#include "Mesher.h"
#include <glm/vec3.hpp>

const int USE_SIZE = 17;

const int USE_SIZE_2 = 289;

using namespace std;
using namespace glm;

bool enableBlockShading_ = true;

char lightCvalue_ = 10;
char lightMvalue_ = 15;

int xDataBitOffset = 0;
int yDataBitOffset = 5;
int zDataBitOffset = 10;
int blockShadingBitOffset = 15;
int textureBitOffset = 20;


bool ChunkMesh::checkUseX(int x, int y, int z) {
	return useX[x * USE_SIZE_2 + y * USE_SIZE + z];
}
bool ChunkMesh::checkUseY(int x, int y, int z) {
	return useY[x * USE_SIZE_2 + y * USE_SIZE + z];
}
bool ChunkMesh::checkUseZ(int x, int y, int z) {
	return useZ[x * USE_SIZE_2 + y * USE_SIZE + z];
}


bool ChunkMesh::checkUseXN(int x, int y, int z) {
	return useXN[x * USE_SIZE_2 + y * USE_SIZE + z];
}
bool ChunkMesh::checkUseYN(int x, int y, int z) {
	return useYN[x * USE_SIZE_2 + y * USE_SIZE + z];
}
bool ChunkMesh::checkUseZN(int x, int y, int z) {
	return useZN[x * USE_SIZE_2 + y * USE_SIZE + z];
}

void ChunkMesh::clear() {
	vertices.clear();
	transparentVertices.clear();
}


void ChunkMesh::SmartGreedyMeshing() {
	if (chunk->isEmpty)
		return;

	SMesh = new BakedChunkData;

	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
			for (int y = 0; y < 16; y++) {
				BlockID block = chunk->GetBlock(x, y, z);
				BlockType blockinfo = *BlockList[block]->block;


				if (blockinfo.isSolid && (!blockinfo.transparency)) {

					if (BlockList[chunk->GetBlock(x - 1, y, z)]->block->transparency)
						SaddSidenx(x, y, z, block);
					if (BlockList[chunk->GetBlock(x + 1, y, z)]->block->transparency)
						SaddSidepx(x, y, z, block);
					if (BlockList[chunk->GetBlock(x, y - 1, z)]->block->transparency)
						SaddSideny(x, y, z, block);
					if (BlockList[chunk->GetBlock(x, y + 1, z)]->block->transparency)
						SaddSidepy(x, y, z, block);
					if (BlockList[chunk->GetBlock(x, y, z - 1)]->block->transparency)
						SaddSidenz(x, y, z, block);
					if (BlockList[chunk->GetBlock(x, y, z + 1)]->block->transparency)
						SaddSidepz(x, y, z, block);
				}

				if (blockinfo.isSolid && (blockinfo.transparency)) {
					if (chunk->GetBlock(x - 1, y, z) == AIR)
						SaddSidenx(x, y, z, block);
					if (chunk->GetBlock(x + 1, y, z) == AIR)
						SaddSidepx(x, y, z, block);
					if (chunk->GetBlock(x, y - 1, z) == AIR)
						SaddSideny(x, y, z, block);
					if (chunk->GetBlock(x, y + 1, z) == AIR)
						SaddSidepy(x, y, z, block);
					if (chunk->GetBlock(x, y, z - 1) == AIR)
						SaddSidenz(x, y, z, block);
					if (chunk->GetBlock(x, y, z + 1) == AIR)
						SaddSidepz(x, y, z, block);

				}

				if (BlockList[block]->block->isFluid) {
					if (BlockList[chunk->GetBlock(x, y + 1, z)]->block->transparency) {
						SaddSidepy(x, y, z, block);
					}
				}
			}
		}
	}
	//Process Data and Gen Mesh

	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {

				int x0 = x;
				int y0 = y;
				int z0 = z;
				int x1 = x;
				int y1 = y;
				int z1 = z;

				unsigned int VDATA[6]{};

				VDATA[PX] = SMesh->extract(x, y, z).Vdata[PX];
				VDATA[NX] = SMesh->extract(x, y, z).Vdata[NX];
				VDATA[PY] = SMesh->extract(x, y, z).Vdata[PY];
				VDATA[NY] = SMesh->extract(x, y, z).Vdata[NY];
				VDATA[PZ] = SMesh->extract(x, y, z).Vdata[PZ];
				VDATA[NZ] = SMesh->extract(x, y, z).Vdata[NZ];

				bool transparent = SMesh->isTransparent(x,y,z);

				BlockID block = SMesh->extract(x, y, z).data;

				//PY
				if (block != AIR) {
					tpy_ = BlockList[SMesh->extract(x,y,z).data]->Texture->GetFace(TOP);
					x0 = x;
					y0 = y;
					z0 = z;
					x1 = x;
					y1 = y;
					z1 = z;
					while (x1 < 16) {
						if ((!checkUseY(x1, y, z)) && (VDATA[PY] == SMesh->extract(x1, y, z).Vdata[PY]) && (SMesh->isTransparent(x1, y + 1, z))) {
							useY[x1 * USE_SIZE_2 + (y)*USE_SIZE + z] = true;
							x1++;
						}
						else { break; }
					}
					while (z1 < 16) {
						z1++;
						bool break_ = false;
						int dx = x0;
						while (dx < x1) {
							if ((!checkUseY(dx, y, z1)) && (VDATA[PY] == SMesh->extract(dx, y, z1).Vdata[PY]) && (SMesh->isTransparent(dx, y + 1, z1))) {
								dx++;
							}
							else {
								break_ = true;
								break;
							}
						}
						if (break_) { break; }
						for (int dx1 = x0; dx1 < x1; dx1++) {
							useY[dx1 * USE_SIZE_2 + (y)*USE_SIZE + z1] = true;
						}
					}
					unsigned int quad = SMesh->extract(x, y, z).Vdata[PY];
					if (!((x0 - x1) == 0 || (z0 - z1) == 0))
						SGaddSidepy(ivec3(x0, y0, z0), ivec3(x1, y1, z1), false, x1 - x, z1 - z, SMesh->extractVData(BMESH_L0, quad), SMesh->extractVData(BMESH_L1, quad), SMesh->extractVData(BMESH_L2, quad), SMesh->extractVData(BMESH_L3, quad));
				}
				//PX
				if (block != AIR) {
					tpx_ = BlockList[SMesh->extract(x, y, z).data]->Texture->GetFace(RIGHT);
					x0 = x;
					y0 = y;
					z0 = z;
					x1 = x;
					y1 = y;
					z1 = z;
					while (y1 < 16) {
						if ((!checkUseX(x, y1, z)) && (VDATA[PX] == SMesh->extract(x, y1, z).Vdata[PX]) && (SMesh->isTransparent(x + 1, y1, z))) {
							useX[x * USE_SIZE_2 + (y1)*USE_SIZE + z] = true;
							y1++;
						}
						else { break; }
					}
					while (z1 < 16) {
						z1++;
						bool break_ = false;
						int dy = y0;
						while (dy < y1) {
							if ((!checkUseX(x, dy, z1)) && (VDATA[PX] == SMesh->extract(x, dy, z1).Vdata[PX]) && (SMesh->isTransparent(x + 1, dy, z1))) {
								dy++;
							}
							else {
								break_ = true;
								break;
							}
						}
						if (break_) { break; }
						for (int dy1 = y0; dy1 < y1; dy1++) {
							useX[x * USE_SIZE_2 + (dy1)*USE_SIZE + z1] = true;
						}
					}
					unsigned int quad = SMesh->extract(x, y, z).Vdata[PX];
					if (!((y0 - y1) == 0 || (z0 - z1) == 0))
						SGaddSidepx(ivec3(x0, y0, z0), ivec3(x1, y1, z1), false, y1 - y, z1 - z, SMesh->extractVData(BMESH_L3, quad), SMesh->extractVData(BMESH_L1, quad), SMesh->extractVData(BMESH_L0, quad), SMesh->extractVData(BMESH_L2, quad));
				}
				//PZ
				if (block != AIR) {
					tpz_ = BlockList[SMesh->extract(x, y, z).data]->Texture->GetFace(FRONT);
					x0 = x;
					y0 = y;
					z0 = z;
					x1 = x;
					y1 = y;
					z1 = z;
					while (y1 < 16) {
						if ((!checkUseZ(x, y1, z)) && (VDATA[PZ] == SMesh->extract(x, y1, z).Vdata[PZ]) && (SMesh->isTransparent(x, y1, z + 1))) {
							useZ[x * USE_SIZE_2 + (y1)*USE_SIZE + z] = true;
							y1++;
						}
						else {
							break;
						}
					}
					while (x1 < 16) {
						x1++;
						bool break_ = false;
						int dy = y0;
						while (dy < y1) {
							if ((!checkUseZ(x1, dy, z)) && (VDATA[PZ] == SMesh->extract(x1, dy, z).Vdata[PZ]) && (SMesh->isTransparent(x1, dy, z + 1))) {
								dy++;
							}
							else {
								break_ = true;
								break;
							}
						}
						if (break_)
							break;
						for (int dy1 = y0; dy1 < y1; dy1++) {
							useZ[x1 * USE_SIZE_2 + (dy1)*USE_SIZE + z] = true;
						}
					}
					unsigned int quad = SMesh->extract(x, y, z).Vdata[PZ];
					if (!((y0 - y1) == 0 || (x0 - x1) == 0))
						SGaddSidepz(ivec3(x0, y0, z0), ivec3(x1, y1, z1), false, x1 - x, y1 - y, SMesh->extractVData(BMESH_L2, quad), SMesh->extractVData(BMESH_L1, quad), SMesh->extractVData(BMESH_L0, quad), SMesh->extractVData(BMESH_L3, quad));
				}


				//NEGATIVES
				//NY
				if (block != AIR) {
					tny_ = BlockList[SMesh->extract(x, y, z).data]->Texture->GetFace(BOTTOM);
					x0 = x;
					y0 = y;
					z0 = z;
					x1 = x;
					y1 = y;
					z1 = z;
					while (x1 < 16) {
						if ((!checkUseYN(x1, y, z)) && (VDATA[NY] == SMesh->extract(x1, y, z).Vdata[NY]) && (SMesh->isTransparent(x1, y - 1, z))) {
							useYN[x1 * USE_SIZE_2 + (y)*USE_SIZE + z] = true;
							x1++;
						}
						else { break; }
					}
					while (z1 < 16) {
						z1++;
						bool break_ = false;
						int dx = x0;
						while (dx < x1) {
							if ((!checkUseYN(dx, y, z1)) && (VDATA[NY] == SMesh->extract(dx, y, z1).Vdata[NY]) && (SMesh->isTransparent(dx, y - 1, z1))) {
								dx++;
							}
							else {
								break_ = true;
								break;
							}
						}
						if (break_) { break; }
						for (int dx1 = x0; dx1 < x1; dx1++) {
							useYN[dx1 * USE_SIZE_2 + (y)*USE_SIZE + z1] = true;
						}
					}
					unsigned int quad = SMesh->extract(x, y, z).Vdata[NY];
					if (!((x0 - x1) == 0 || (z0 - z1) == 0))
						SGaddSideny(ivec3(x0, y0, z0), ivec3(x1, y1, z1), false, x1 - x, z1 - z, SMesh->extractVData(BMESH_L0, quad), SMesh->extractVData(BMESH_L1, quad), SMesh->extractVData(BMESH_L2, quad), SMesh->extractVData(BMESH_L3, quad));
				}
				//NX
				if (block != AIR) {
					tnx_ = BlockList[SMesh->extract(x, y, z).data]->Texture->GetFace(LEFT);
					x0 = x;
					y0 = y;
					z0 = z;
					x1 = x;
					y1 = y;
					z1 = z;
					while (y1 < 16) {
						if ((!checkUseXN(x, y1, z)) && (VDATA[NX] == SMesh->extract(x, y1, z).Vdata[NX]) && (SMesh->isTransparent(x - 1, y1, z))) {
							useXN[x * USE_SIZE_2 + (y1)*USE_SIZE + z] = true;
							y1++;
						}
						else { break; }
					}
					while (z1 < 16) {
						z1++;
						bool break_ = false;
						int dy = y0;
						while (dy < y1) {
							if ((!checkUseXN(x, dy, z1)) && (VDATA[NX] == SMesh->extract(x, dy, z1).Vdata[NX]) && (SMesh->isTransparent(x - 1, dy, z1))) {
								dy++;
							}
							else {
								break_ = true;
								break;
							}
						}
						if (break_) { break; }
						for (int dy1 = y0; dy1 < y1; dy1++) {
							useXN[x * USE_SIZE_2 + (dy1)*USE_SIZE + z1] = true;
						}
					}
					unsigned int quad = SMesh->extract(x, y, z).Vdata[NX];
					if (!((y0 - y1) == 0 || (z0 - z1) == 0))
						SGaddSidenx(ivec3(x0, y0, z0), ivec3(x1, y1, z1), false, y1 - y, z1 - z, SMesh->extractVData(BMESH_L3, quad), SMesh->extractVData(BMESH_L1, quad), SMesh->extractVData(BMESH_L0, quad), SMesh->extractVData(BMESH_L2, quad));
				}
				//NZ
				if (block != AIR) {
					tnz_ = BlockList[SMesh->extract(x, y, z).data]->Texture->GetFace(BACK);
					x0 = x;
					y0 = y;
					z0 = z;
					x1 = x;
					y1 = y;
					z1 = z;
					while (y1 < 16) {
						if ((!checkUseZN(x, y1, z)) && (VDATA[NZ] == SMesh->extract(x, y1, z).Vdata[NZ]) && (SMesh->isTransparent(x, y1, z - 1))) {
							useZN[x * USE_SIZE_2 + (y1)*USE_SIZE + z] = true;
							y1++;
						}
						else {
							break;
						}
					}
					while (x1 < 16) {
						x1++;
						bool break_ = false;
						int dy = y0;
						while (dy < y1) {
							if ((!checkUseZN(x1, dy, z)) && (VDATA[NZ] == SMesh->extract(x1, dy, z).Vdata[NZ]) && (SMesh->isTransparent(x1, dy, z - 1))) {
								dy++;
							}
							else {
								break_ = true;
								break;
							}
						}
						if (break_)
							break;
						for (int dy1 = y0; dy1 < y1; dy1++) {
							useZN[x1 * USE_SIZE_2 + (dy1)*USE_SIZE + z] = true;
						}
					}
					unsigned int quad = SMesh->extract(x, y, z).Vdata[NZ];
					if (!((y0 - y1) == 0 || (x0 - x1) == 0))
						SGaddSidenz(ivec3(x0, y0, z0), ivec3(x1, y1, z1), false, y1 - y , x1 - x, SMesh->extractVData(BMESH_L2, quad), SMesh->extractVData(BMESH_L1, quad), SMesh->extractVData(BMESH_L0, quad), SMesh->extractVData(BMESH_L3, quad));
				}

			}
		}

	}
	for (int i = 0; i < USE_SIZE * USE_SIZE * USE_SIZE; i++) {
		useX[i] = false;
		useY[i] = false;
		useZ[i] = false;
		useXN[i] = false;
		useYN[i] = false;
		useZN[i] = false;
	}

	delete SMesh;
}

void ChunkMesh::SGaddSidepy(ivec3 p0, ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1) {

	if (a + b == 0) {
		return;
	}

	if (transparency) {
		transparentVertices.push_back(0u | (p0.x) | ((1 + p0.y) << yDataBitOffset) | (p1.z << zDataBitOffset) | (b << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (sy << 10) | (tpy_ << textureBitOffset));
		transparentVertices.push_back(0u | (p0.x) | ((1 + p0.y) << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (0 << 10) | (tpy_ << textureBitOffset));
		transparentVertices.push_back(0u | (p1.x) | ((1 + p0.y) << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (sy << 10) | (tpy_ << textureBitOffset));
		transparentVertices.push_back(0u | (p0.x) | ((1 + p0.y) << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (0 << 10) | (tpy_ << textureBitOffset));
		transparentVertices.push_back(0u | (p1.x) | ((1 + p0.y) << yDataBitOffset) | (p0.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (0 << 10) | (tpy_ << textureBitOffset));
		transparentVertices.push_back(0u | (p1.x) | ((1 + p0.y) << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (sy << 10) | (tpy_ << textureBitOffset));
	}
	else {
		vertices.push_back(0u | (p0.x) | ((1 + p0.y) << yDataBitOffset) | (p1.z << zDataBitOffset) | (b << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (sy << 10) | (tpy_ << textureBitOffset));
		vertices.push_back(0u | (p0.x) | ((1 + p0.y) << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (0 << 10) | (tpy_ << textureBitOffset));
		vertices.push_back(0u | (p1.x) | ((1 + p0.y) << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (sy << 10) | (tpy_ << textureBitOffset));
		vertices.push_back(0u | (p0.x) | ((1 + p0.y) << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (0 << 10) | (tpy_ << textureBitOffset));
		vertices.push_back(0u | (p1.x) | ((1 + p0.y) << yDataBitOffset) | (p0.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (0 << 10) | (tpy_ << textureBitOffset));
		vertices.push_back(0u | (p1.x) | ((1 + p0.y) << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (sy << 10) | (tpy_ << textureBitOffset));
	}
}
void ChunkMesh::SGaddSideny(ivec3 p0, ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1) {
	if (a + b == 0) {
		return;
	}
	if (transparency) {
		transparentVertices.push_back(0u | (p0.x) | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (0 << 10) | (tny_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | (p0.x) | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (b << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (sy << 10) | (tny_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | (p1.x) | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (sy << 10) | (tny_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | (p1.x) | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (0 << 10) | (tny_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | (p0.x) | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (0 << 10) | (tny_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | (p1.x) | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (sy << 10) | (tny_ << textureBitOffset) | (1 << 31));
	}
	else {
		vertices.push_back(0u | (p0.x) | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (0 << 10) | (tny_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | (p0.x) | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (b << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (sy << 10) | (tny_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | (p1.x) | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (sy << 10) | (tny_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | (p1.x) | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (0 << 10) | (tny_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | (p0.x) | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (0 << 10) | (tny_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | (p1.x) | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (sy << 10) | (tny_ << textureBitOffset) | (1 << 31));
	}

}
void ChunkMesh::SGaddSidepx(ivec3 p0, ivec3 p1, bool transparency, int sy, int sx, int a, int b, int a1, int b1) {
	if (a + b == 0) {
		return;
	}
	if (transparency) {
		transparentVertices.push_back(0u | p0.x + 1 | (p1.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (0 << 10) | (tpx_ << textureBitOffset));
		transparentVertices.push_back(0u | p0.x + 1 | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (0 << 10) | (tpx_ << textureBitOffset));
		transparentVertices.push_back(0u | p0.x + 1 | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (sy << 10) | (tpx_ << textureBitOffset));
		transparentVertices.push_back(0u | p0.x + 1 | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (0 << 10) | (tpx_ << textureBitOffset));
		transparentVertices.push_back(0u | p0.x + 1 | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (sy << 10) | (tpx_ << textureBitOffset));
		transparentVertices.push_back(0u | p0.x + 1 | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (sy << 10) | (tpx_ << textureBitOffset));
	}
	else {
		vertices.push_back(0u | p0.x + 1 | (p1.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (0 << 10) | (tpx_ << textureBitOffset));
		vertices.push_back(0u | p0.x + 1 | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (0 << 10) | (tpx_ << textureBitOffset));
		vertices.push_back(0u | p0.x + 1 | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (sy << 10) | (tpx_ << textureBitOffset));
		vertices.push_back(0u | p0.x + 1 | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (0 << 10) | (tpx_ << textureBitOffset));
		vertices.push_back(0u | p0.x + 1 | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (sy << 10) | (tpx_ << textureBitOffset));
		vertices.push_back(0u | p0.x + 1 | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (sy << 10) | (tpx_ << textureBitOffset));
	}

}
void ChunkMesh::SGaddSidenx(ivec3 p0, ivec3 p1, bool transparency, int sy, int sx, int a, int b, int a1, int b1) {
	if (a + b == 0) {
		return;
	}
	if (transparency) {
		transparentVertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (0 << 10) | (tnx_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (0 << 10) | (tnx_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | p0.x | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (sy << 10) | (tnx_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | p0.x | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (sy << 10) | (tnx_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (0 << 10) | (tnx_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | p0.x | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sx << 0) | (sy << 10) | (tnx_ << textureBitOffset) | (1 << 31));
	}
	else {
		vertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (0 << 10) | (tnx_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (0 << 10) | (tnx_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | p0.x | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (sy << 10) | (tnx_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | p0.x | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (sy << 10) | (tnx_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (0 << 10) | (tnx_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | p0.x | (p0.y << yDataBitOffset) | (p1.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		vertices.push_back(0u | (sx << 0) | (sy << 10) | (tnx_ << textureBitOffset) | (1 << 31));
	}

}
void ChunkMesh::SGaddSidepz(ivec3 p0, ivec3 p1, bool transparency, int sy, int sx, int a, int b, int a1, int b1) {
	if (a + b == 0) {
		return;
	}
	if (transparency) {
		transparentVertices.push_back(0u | p1.x | (p1.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (a << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (0 << 10) | (tpz_ << textureBitOffset));
		transparentVertices.push_back(0u | p1.x | (p0.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (b << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (sx << 10) | (tpz_ << textureBitOffset));
		transparentVertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (b1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sy << 0) | (0 << 10) | (tpz_ << textureBitOffset));
		transparentVertices.push_back(0u | p1.x | (p0.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (b << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (sx << 10) | (tpz_ << textureBitOffset));
		transparentVertices.push_back(0u | p0.x | (p0.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (a1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sy << 0) | (sx << 10) | (tpz_ << textureBitOffset));
		transparentVertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (b1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sy << 0) | (0 << 10) | (tpz_ << textureBitOffset));
	}
	else {
		vertices.push_back(0u | p1.x | (p1.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (a << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (0 << 10) | (tpz_ << textureBitOffset));
		vertices.push_back(0u | p1.x | (p0.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (b << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (sx << 10) | (tpz_ << textureBitOffset));
		vertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (b1 << blockShadingBitOffset));
		vertices.push_back(0u | (sy << 0) | (0 << 10) | (tpz_ << textureBitOffset));
		vertices.push_back(0u | p1.x | (p0.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (b << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (sx << 10) | (tpz_ << textureBitOffset));
		vertices.push_back(0u | p0.x | (p0.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (a1 << blockShadingBitOffset));
		vertices.push_back(0u | (sy << 0) | (sx << 10) | (tpz_ << textureBitOffset));
		vertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | ((p0.z + 1) << zDataBitOffset) | (b1 << blockShadingBitOffset));
		vertices.push_back(0u | (sy << 0) | (0 << 10) | (tpz_ << textureBitOffset));
	}

}
void ChunkMesh::SGaddSidenz(ivec3 p0, ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1) {
	if (a + b == 0) {
		return;
	}
	if (transparency) {
		transparentVertices.push_back(0u | p1.x | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (sx << 10) | (tnz_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | p1.x | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (0 << 10) | (tnz_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sy << 0) | (0 << 10) | (tnz_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | p0.x | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sy << 0) | (sx << 10) | (tnz_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | p1.x | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b << blockShadingBitOffset));
		transparentVertices.push_back(0u | (0 << 0) | (sx << 10) | (tnz_ << textureBitOffset) | (1 << 31));
		transparentVertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		transparentVertices.push_back(0u | (sy << 0) | (0 << 10) | (tnz_ << textureBitOffset) | (1 << 31));

	}
	else {
		vertices.push_back(0u | p1.x | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (sx << 10) | (tnz_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | p1.x | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (0 << 10) | (tnz_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		vertices.push_back(0u | (sy << 0) | (0 << 10) | (tnz_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | p0.x | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (a1 << blockShadingBitOffset));
		vertices.push_back(0u | (sy << 0) | (sx << 10) | (tnz_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | p1.x | (p0.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b << blockShadingBitOffset));
		vertices.push_back(0u | (0 << 0) | (sx << 10) | (tnz_ << textureBitOffset) | (1 << 31));
		vertices.push_back(0u | p0.x | (p1.y << yDataBitOffset) | (p0.z << zDataBitOffset) | (b1 << blockShadingBitOffset));
		vertices.push_back(0u | (sy << 0) | (0 << 10) | (tnz_ << textureBitOffset) | (1 << 31));
	}

}

void ChunkMesh::SaddSideny(int x, int y, int z, BlockID block) {
	int a = lightMvalue_;
	int b = lightMvalue_;
	int a1 = lightMvalue_;
	int b1 = lightMvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y - 1, z)]->block->transparency) {
		a = lightCvalue_;
		b = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y - 1, z)]->block->transparency) {
		a1 = lightCvalue_;
		b1 = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x, y - 1, z + 1)]->block->transparency) {
		a1 = lightCvalue_;
		b = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x, y - 1, z - 1)]->block->transparency) {
		b1 = lightCvalue_;
		a = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y - 1, z + 1)]->block->transparency)
		a1 = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y - 1, z - 1)]->block->transparency)
		a = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x + 1, y - 1, z - 1)]->block->transparency)
		b1 = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y - 1, z + 1)]->block->transparency)
		b = lightCvalue_;
	SMesh->add(x, y, z, block, a, b, a1, b1, NY);
}
void ChunkMesh::SaddSidepy(int x, int y, int z, BlockID block) {
	int a = lightMvalue_;
	int b = lightMvalue_;
	int a1 = lightMvalue_;
	int b1 = lightMvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y + 1, z)]->block->transparency) {
		a = lightCvalue_;
		b = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y + 1, z)]->block->transparency) {
		a1 = lightCvalue_;
		b1 = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x, y + 1, z + 1)]->block->transparency) {
		a1 = lightCvalue_;
		b = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x, y + 1, z - 1)]->block->transparency) {
		b1 = lightCvalue_;
		a = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y + 1, z + 1)]->block->transparency)
		a1 = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y + 1, z - 1)]->block->transparency)
		a = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x + 1, y + 1, z - 1)]->block->transparency)
		b1 = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y + 1, z + 1)]->block->transparency)
		b = lightCvalue_;
	SMesh->add(x, y, z, block, a, b, a1, b1, PY);
}
void ChunkMesh::SaddSidenx(int x, int y, int z, BlockID block) {
	int br = lightMvalue_;
	int bl = lightMvalue_;
	int tl = lightMvalue_;
	int tr = lightMvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y - 1, z)]->block->transparency) {
		br = lightCvalue_;
		bl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x - 1, y + 1, z)]->block->transparency) {
		tr = lightCvalue_;
		tl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x - 1, y, z + 1)]->block->transparency) {
		tr = lightCvalue_;
		br = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x - 1, y, z - 1)]->block->transparency) {
		bl = lightCvalue_;
		tl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x - 1, y + 1, z + 1)]->block->transparency)
		tr = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y + 1, z - 1)]->block->transparency)
		tl = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y - 1, z - 1)]->block->transparency)
		bl = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y - 1, z + 1)]->block->transparency)
		br = lightCvalue_;
	SMesh->add(x, y, z, block, br, bl, tr, tl, NX);
}
void ChunkMesh::SaddSidepx(int x, int y, int z, BlockID block) {
	int br = lightMvalue_;
	int bl = lightMvalue_;
	int tl = lightMvalue_;
	int tr = lightMvalue_;
	if (!BlockList[chunk->GetBlock(x + 1, y - 1, z)]->block->transparency) {
		br = lightCvalue_;
		bl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y + 1, z)]->block->transparency) {
		tr = lightCvalue_;
		tl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y, z + 1)]->block->transparency) {
		tr = lightCvalue_;
		br = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y, z - 1)]->block->transparency) {
		bl = lightCvalue_;
		tl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y + 1, z + 1)]->block->transparency)
		tr = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x + 1, y + 1, z - 1)]->block->transparency)
		tl = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x + 1, y - 1, z - 1)]->block->transparency)
		bl = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x + 1, y - 1, z + 1)]->block->transparency)
		br = lightCvalue_;
	SMesh->add(x, y, z, block, br, bl, tr, tl, PX); // (bl,br,tl,tr)
}
void ChunkMesh::SaddSidenz(int x, int y, int z, BlockID block) {
	int br = lightMvalue_;
	int bl = lightMvalue_;
	int tl = lightMvalue_;
	int tr = lightMvalue_;
	if (!BlockList[chunk->GetBlock(x, y - 1, z - 1)]->block->transparency) {
		br = lightCvalue_;
		bl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x, y + 1, z - 1)]->block->transparency) {
		tr = lightCvalue_;
		tl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y, z - 1)]->block->transparency) {
		tl = lightCvalue_;
		bl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x - 1, y, z - 1)]->block->transparency) {
		br = lightCvalue_;
		tr = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y + 1, z - 1)]->block->transparency)
		tl = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y + 1, z - 1)]->block->transparency)
		tr = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y - 1, z - 1)]->block->transparency)
		br = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x + 1, y - 1, z - 1)]->block->transparency)
		bl = lightCvalue_;
	SMesh->add(x, y, z, block, br, bl, tl, tr, NZ); // (br,bf,tf,tr)
}
void ChunkMesh::SaddSidepz(int x, int y, int z, BlockID block) {
	int br = lightMvalue_;
	int bl = lightMvalue_;
	int tl = lightMvalue_;
	int tr = lightMvalue_;
	if (!BlockList[chunk->GetBlock(x, y - 1, z + 1)]->block->transparency) {
		br = lightCvalue_;
		bl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x, y + 1, z + 1)]->block->transparency) {
		tr = lightCvalue_;
		tl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y, z + 1)]->block->transparency) {
		tl = lightCvalue_;
		bl = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x - 1, y, z + 1)]->block->transparency) {
		br = lightCvalue_;
		tr = lightCvalue_;
	}
	if (!BlockList[chunk->GetBlock(x + 1, y + 1, z + 1)]->block->transparency)
		tl = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y + 1, z + 1)]->block->transparency)
		tr = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x - 1, y - 1, z + 1)]->block->transparency)
		br = lightCvalue_;
	if (!BlockList[chunk->GetBlock(x + 1, y - 1, z + 1)]->block->transparency)
		bl = lightCvalue_;
	SMesh->add(x, y, z, block, br, bl, tl, tr, PZ);
}