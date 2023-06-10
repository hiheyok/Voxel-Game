#pragma once
#ifndef MESH_H_2
#define MESH_H_2


#include "../../../World/Chunk/Chunk.h"

#include <vector>

#include "../../../utils/LogUtils.h"
#include <glm/vec3.hpp>

#include "BakedChunkMesh.h"

#include <vector>

#include <unordered_map>



/*
BLOCK DATA (32 BIT):
BIT_OFFSET		TYPE			SIZE
0				ID				8
8				Transparency	1
8				LIGHT			4


*/

/* 
VDATA (32 BIT):
BIT_OFFSET		TYPE			SIZE
0				ID				8
8				L0				4
12				L1				4
16				L2				4
20				L3				4
*/

//For indexing stuff
#define SOLID 0x00			
#define TRANSPARENT 0x01	

class ChunkMesh {
public:

	Chunk* chunk = nullptr;

	void SmartGreedyMeshing();

	void GreedyMeshing();

	std::vector<unsigned int> vertices;
	std::vector<unsigned int> transparentVertices;

	void clear();

private:

	void SaddSidenx(int x, int y, int z, BlockID block);
	void SaddSidepx(int x, int y, int z, BlockID block);

	void SaddSideny(int x, int y, int z, BlockID block);
	void SaddSidepy(int x, int y, int z, BlockID block);

	void SaddSidenz(int x, int y, int z, BlockID block);
	void SaddSidepz(int x, int y, int z, BlockID block);

	void SGaddSidenx(glm::ivec3 p0, glm::ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1);
	void SGaddSidepx(glm::ivec3 p0, glm::ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1);

	void SGaddSideny(glm::ivec3 p0, glm::ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1);
	void SGaddSidepy(glm::ivec3 p0, glm::ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1);

	void SGaddSidenz(glm::ivec3 p0, glm::ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1);
	void SGaddSidepz(glm::ivec3 p0, glm::ivec3 p1, bool transparency, int sy, int sx, int a, int b, int a1, int b1);

	unsigned int tnx_ = 0;
	unsigned int tpx_ = 0;
	unsigned int tny_ = 0;
	unsigned int tpy_ = 0;
	unsigned int tnz_ = 0;
	unsigned int tpz_ = 0;

	bool checkUseX(int x, int y, int z);
	bool checkUseY(int x, int y, int z);
	bool checkUseZ(int x, int y, int z);
	bool checkUseYN(int x, int y, int z);
	bool checkUseXN(int x, int y, int z);
	bool checkUseZN(int x, int y, int z);

	bool useX[(16 + 1) * (16 + 1) * (16 + 1)]{};
	bool useY[(16 + 1) * (16 + 1) * (16 + 1)]{};
	bool useZ[(16 + 1) * (16 + 1) * (16 + 1)]{};

	bool useXN[(16 + 1) * (16 + 1) * (16 + 1)]{};
	bool useYN[(16 + 1) * (16 + 1) * (16 + 1)]{};
	bool useZN[(16 + 1) * (16 + 1) * (16 + 1)]{};

	BakedChunkData* SMesh = nullptr;
};



#endif