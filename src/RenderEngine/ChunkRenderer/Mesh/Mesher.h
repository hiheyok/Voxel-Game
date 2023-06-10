#ifndef MESH_H_2
#define MESH_H_2
#define _CRTDBG_MAP_ALLOC


#include "../../../World/Chunk/Chunk.h"
#include "../../../utils/MathHelper.h"

#include <unordered_map>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>

#include "../../../utils/FastNoiseLite.h"
#include "../../../utils/MathHelper.h"
#include "../../../utils/Clock.h"
#include "../../../utils/LogUtils.h"
#include "../../../Utils/Math/Vector/vec3.h"

#include "BakedChunkMesh.h"

#include <vector>
#include <algorithm>

#include <unordered_map>
#include <string>



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

	void SaddSidenx(int x, int y, int z, Block block);
	void SaddSidepx(int x, int y, int z, Block block);

	void SaddSideny(int x, int y, int z, Block block);
	void SaddSidepy(int x, int y, int z, Block block);

	void SaddSidenz(int x, int y, int z, Block block);
	void SaddSidepz(int x, int y, int z, Block block);

	void SGaddSidenx(ivec3 p0, ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1);
	void SGaddSidepx(ivec3 p0, ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1);

	void SGaddSideny(ivec3 p0, ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1);
	void SGaddSidepy(ivec3 p0, ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1);

	void SGaddSidenz(ivec3 p0, ivec3 p1, bool transparency, int sx, int sy, int a, int b, int a1, int b1);
	void SGaddSidepz(ivec3 p0, ivec3 p1, bool transparency, int sy, int sx, int a, int b, int a1, int b1);

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