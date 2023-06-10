#pragma once

#include "../../../World/Chunk/Chunk.h"

constexpr auto BMESH_ID = 0x00;
constexpr auto BMESH_L0 = 0x01;
constexpr auto BMESH_L1 = 0x02;
constexpr auto BMESH_L2 = 0x03;
constexpr auto BMESH_L3 = 0x04;

struct BlockVerticesData {
	BlockVerticesData(Block id = AIR) : data(id) {}
	unsigned int Vdata[6]{};
	Block data;
};

struct BakedChunkData {
	void addUninitBlock(int x, int y, int z, Block B_ID);
	BlockVerticesData extract(int x, int y, int z);
	BlockID GetBlockID(int x, int y, int z);
	bool compare(int x, int y, int z, int x1, int y1, int z1, char type);
	bool isTransparent(int x, int y, int z);
	unsigned int extractVData(char type, unsigned int data);
	BlockVerticesData data[4096]{};
	void add(int x, int y, int z, Block block, unsigned int L0_, unsigned int L1_, unsigned int L2_, unsigned int L3_, char side);
};
