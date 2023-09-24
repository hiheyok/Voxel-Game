#pragma once
#include "../Chunk/Block/Blocks.h"
#include <glm/vec3.hpp>

class BlockContainer {
public:
	
	inline BlockID GetBlock(unsigned int x, unsigned int y, unsigned int z) {
		return Data[GetDataIndex(x,y,z)];
	}

	inline void ChangeBlock(BlockID ID, unsigned int x, unsigned int y, unsigned int z) {
		Data[GetDataIndex(x,y,z)] = ID;
	}

	void DumpData() {
		std::ofstream file;
		file.open("Chunk.dmp");
		for (int i = 0; i < 4096; i++) {
			file << Data[i];
		}
		file.close();
	}


private:

	inline static unsigned int GetDataIndex(unsigned int x, unsigned int y, unsigned int z) {
		return (x << 8) + (z << 4) + y;
	}
	
	BlockID Data[4096]{AIR};
};

//namespace BinaryStorage {
//	struct Node {
//		bool isLeaf = false;
//		BlockID data;
//		Node* Addresses[2]{ nullptr, nullptr };
//	};
//
//	struct NodeData {
//		BlockID data;
//	};
//}
//
//class BlockContainerCompress {
//	BlockID GetBlock(unsigned int x, unsigned int y, unsigned int z) {
//
//		unsigned int p[3]{x,y,z};
//
//		const unsigned int ChunkSize = 16;
//
//		int searchDepth = 0;
//
//		bool stop = false;
//
//		BinaryStorage::Node* node;
//
//		node = &main;
//
//		while (stop) {
//			if (node->isLeaf) {
//				if (p[searchDepth % 3] < 8) {
//					node = node->Addresses[0];
//				}
//				else {
//					node = node->Addresses[1];
//					p[searchDepth % 3] = p[searchDepth % 3] & ((~0b0) & (~(ChunkSize >> (int)floor((float)searchDepth / 3.f))));
//				}
//				
//			}
//			else {
//				return node->data;
//			}
//		}
//
//	}
//
//	void ChangeBlock(BlockID ID, unsigned int x, unsigned int y, unsigned int z) {
//		//Data[GetDataIndex(x, y, z)] = ID;
//	}
//
//
//
//
//private:
//
//	static unsigned int GetDataIndex(unsigned int x, unsigned int y, unsigned int z) {
//		return (x * 256) + (z * 16) + y;
//	}
//
//	BinaryStorage::Node main;
//};