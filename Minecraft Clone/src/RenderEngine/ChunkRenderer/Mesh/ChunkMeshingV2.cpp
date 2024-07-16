#include "ChunkMeshingV2.h"
#include <immintrin.h>
#include "../../../Level/Chunk/Block/Blocks.h"
#include <unordered_set>
#include <bit> 

using namespace std;
using namespace glm;

const int BitShiftAmount = 9;
const int blockShadingBitOffset = 15;
const int textureBitOffset = 10;
const int NumTextureBitOffset = 22;

void MeshingV2::ChunkMeshData::GenerateMesh(Chunk* chunk) {
	//Initialize
	if (chunk == nullptr) {
		return;
	}
	Position = chunk->Position;
	chunk->Use();
	GenerateFaceCollection(chunk);
	chunk->Unuse();

}

//Checks if there are anything different between q0 and q1

//Loops through all the blocks in the chunk and check if each block side is visible. If a block side is visible, it generates the quad and puts it in the cache

void MeshingV2::ChunkMeshData::GenerateFaceCollection(Chunk* chunk) {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				AddBlock(chunk, x, y, z);
			}
		}
	}
}

void MeshingV2::ChunkMeshData::AddBlock(Chunk* chunk,int x, int y, int z) {
	BlockID b = chunk->GetBlockUnsafe(x, y, z);
	if (b == Blocks.AIR) return;
	Block* block = Blocks.getBlockType(b);
	ModelV2::BlockModelV2* model = block->BlockModelData;
	if (model == NULL) return;

	for (Cuboid& element : model->Elements) {

		for (int direction = 0; direction < 6; direction++) {
			if (element.Faces[direction].ReferenceTexture.length() == 0) continue;// means that there isnt a face here

			if (element.Faces[direction].CullFace != -1) {

				if (!IsFaceVisible(chunk, x, y, z, element.Faces[direction].CullFace))
					continue;
			}

			AddFacetoMesh(element.Faces[direction], direction, element.From, element.To, x, y, z);
		}
	}
	
}


inline void MeshingV2::ChunkMeshData::AddFacetoMesh(BlockFace& face, uint8_t axis, glm::ivec3 From, glm::ivec3 To, int x, int y, int z) {

	switch (axis >> 1) {
	case 0:
		AddFacetoMesh_X(face, axis & 0b1, From, To, x, y, z);
		return;
	case 1:
		AddFacetoMesh_Y(face, axis & 0b1, From, To, x, y, z);
		return;
	case 2:
		AddFacetoMesh_Z(face, axis & 0b1, From, To, x, y, z);
		return;
	}
}

inline void MeshingV2::ChunkMeshData::AddFacetoMesh_X(BlockFace& face, uint8_t direction, glm::ivec3 From, glm::ivec3 To, int x, int y, int z) {
	x *= 16;
	y *= 16;
	z *= 16;

	glm::ivec3 P0{ x + From[0], y + From[1], z + From[2] };
	glm::ivec3 P1{ x + To[0], y + To[1], z + To[2] };


	char NN = 0, PN = 0, PP = 0, NP = 0;

	//Bitshift for compression purposes
	for (int i = 0; i < 3; i++) {
		P0[i] = P0[i] << (i * BitShiftAmount);
		P1[i] = P1[i] << (i * BitShiftAmount);
	}

	uint32_t sx0 = face.UV.x << 0;
	uint32_t sy0 = face.UV.y << 5;
	uint32_t sx = face.UV.z << 0;
	uint32_t sy = face.UV.w << 5;

	uint32_t tex = face.TextureID << 10;
	uint32_t NumTex = 0;

	vector<uint32_t>* out = face.hasTransparency ? &TransparentVertices : &Vertices;

	switch (direction) {
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
		0u | P1[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset)
		,0u | sx0 | sy0 | tex | NumTex
		,0u | P1[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P1[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
		,0u | P1[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset)
		,0u | sx | sy0 | tex | NumTex
		,0u | P1[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset)
		,0u | sx | sy | tex | NumTex
		,0u | P1[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset)
		,0u | sx0 | sy | tex | NumTex
			});
		break;
	}
}

inline void MeshingV2::ChunkMeshData::AddFacetoMesh_Y(BlockFace& face, uint8_t direction, glm::ivec3 From, glm::ivec3 To, int x, int y, int z) {
	x *= 16;
	y *= 16;
	z *= 16;

	glm::ivec3 P0{ x + From[0], y + From[1], z + From[2] };
	glm::ivec3 P1{ x + To[0], y + To[1], z + To[2] };

	char NN = 0, PN = 0, PP = 0, NP = 0;

	//Bitshift for compression purposes
	for (int i = 0; i < 3; i++) {
		P0[i] = P0[i] << (i * BitShiftAmount);
		P1[i] = P1[i] << (i * BitShiftAmount);
	}

	uint32_t sx0 = face.UV.x << 0;
	uint32_t sy0 = face.UV.y << 5;
	uint32_t sx = face.UV.z << 0;
	uint32_t sy = face.UV.w << 5;

	uint32_t tex = face.TextureID << 10;
	uint32_t NumTex = 0;

	vector<uint32_t>* out = face.hasTransparency ? &TransparentVertices : &Vertices;

	switch (direction) {
	case 1:
		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset) //0
			,0u | sx0 | sy0 | tex | NumTex
			,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset) //2
			,0u | sx0 | sy | tex | NumTex
			,0u | P1[0] | P0[1] | P0[2] | (PN << blockShadingBitOffset) //1
			,0u | sx | sy0 | tex | NumTex
			,0u | P0[0] | P0[1] | P1[2] | (NP << blockShadingBitOffset) //3
			,0u | sx0 | sy | tex | NumTex
			,0u | P1[0] | P0[1] | P1[2] | (PP << blockShadingBitOffset) //5
			,0u | sx | sy | tex | NumTex
			,0u | P1[0] | P0[1] | P0[2] | (PN << blockShadingBitOffset) //4
			,0u | sx | sy0 | tex | NumTex
			});
		break;
	case 0:
		out->insert(out->end(), {
			0u | P0[0] | P1[1] | P0[2] | (NN << blockShadingBitOffset) //0
			,0u | sx0 | sy0 | tex | NumTex
			,0u | P1[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset) //1
			,0u | sx | sy0 | tex | NumTex
			,0u | P0[0] | P1[1] | P1[2] | (NP << blockShadingBitOffset) //2
			,0u | sx0 | sy | tex | NumTex
			,0u | P0[0] | P1[1] | P1[2] | (NP << blockShadingBitOffset) //3
			,0u | sx0 | sy | tex | NumTex
			,0u | P1[0] | P1[1] | P0[2] | (PN << blockShadingBitOffset) //4
			,0u | sx | sy0 | tex | NumTex
			,0u | P1[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset) //5
			,0u | sx | sy | tex | NumTex
			});
		
		break;
	}
}

inline void MeshingV2::ChunkMeshData::AddFacetoMesh_Z(BlockFace& face, uint8_t direction, glm::ivec3 From, glm::ivec3 To, int x, int y, int z) { //x : x
	x *= 16;
	y *= 16;
	z *= 16;

	glm::ivec3 P0{ x + From[0], y + From[1], z + From[2] };
	glm::ivec3 P1{ x + To[0], y + To[1], z + To[2] };


	char NN = 0, PN = 0, PP = 0, NP = 0;

	//Bitshift for compression purposes
	for (int i = 0; i < 3; i++) {
		P0[i] = P0[i] << (i * BitShiftAmount);
		P1[i] = P1[i] << (i * BitShiftAmount);
	}

	uint32_t sx0 = face.UV.x << 0;
	uint32_t sy0 = face.UV.y << 5;
	uint32_t sx = face.UV.z << 0;
	uint32_t sy = face.UV.w << 5;

	uint32_t tex = face.TextureID << 10;
	uint32_t NumTex = 0;

	vector<uint32_t>* out = face.hasTransparency ? &TransparentVertices : &Vertices;

	switch (direction) {
	case 1:
		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | (NN << blockShadingBitOffset) //0
			,0u | sx0 | sy0 | tex | NumTex
			,0u | P1[0] | P0[1] | P0[2] | (PN << blockShadingBitOffset) //1
			,0u | sx | sy0 | tex | NumTex
			,0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset) //2
			,0u | sx0 | sy | tex | NumTex
			,0u | P0[0] | P1[1] | P0[2] | (NP << blockShadingBitOffset) //3
			,0u | sx0 | sy | tex | NumTex
			,0u | P1[0] | P0[1] | P0[2] | (PN << blockShadingBitOffset) //4
			,0u | sx | sy0 | tex | NumTex
			,0u | P1[0] | P1[1] | P0[2] | (PP << blockShadingBitOffset) //5
			,0u | sx | sy | tex | NumTex
			});
		break;
	case 0:
		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P1[2] | (NN << blockShadingBitOffset) //0
			,0u | sx0 | sy0 | tex | NumTex
			,0u | P0[0] | P1[1] | P1[2] | (NP << blockShadingBitOffset) //2
			,0u | sx0 | sy | tex | NumTex
			,0u | P1[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset) //1
			,0u | sx | sy0 | tex | NumTex
			,0u | P0[0] | P1[1] | P1[2] | (NP << blockShadingBitOffset) //3
			,0u | sx0 | sy | tex | NumTex
			,0u | P1[0] | P1[1] | P1[2] | (PP << blockShadingBitOffset) //5
			,0u | sx | sy | tex | NumTex
			,0u | P1[0] | P0[1] | P1[2] | (PN << blockShadingBitOffset) //4
			,0u | sx | sy0 | tex | NumTex
			});
		break;
	}
}

//Checks if a block side is visible to the player
inline bool MeshingV2::ChunkMeshData::IsFaceVisible(Chunk* chunk, int x, int y, int z, uint8_t side) {
	//IsFaceVisibleCalls++;

	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return Blocks.getBlockType(chunk->GetBlock(p[0], p[1], p[2]))->Properties->transparency;
}

inline bool MeshingV2::ChunkMeshData::IsFaceVisibleUnsafe(Chunk* chunk, int x, int y, int z, uint8_t side) {
	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return Blocks.getBlockType(chunk->GetBlockUnsafe(p[0], p[1], p[2]))->Properties->transparency;
}

inline bool MeshingV2::ChunkMeshData::CompareBlockSide(Chunk* chunk, int x, int y, int z, uint8_t side, BlockID b) {
	//IsFaceVisibleCalls++;

	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return chunk->GetBlock(p[0], p[1], p[2]) == b;
}

inline bool MeshingV2::ChunkMeshData::CompareBlockSideUnsafe(Chunk* chunk, int x, int y, int z, uint8_t side, BlockID b) {
	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return chunk->GetBlockUnsafe(p[0], p[1], p[2]) == b;
}