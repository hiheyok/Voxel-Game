#include "ChunkMeshingV2.h"
#include <immintrin.h>
#include "../../../Level/Chunk/Block/Blocks.h"
#include <unordered_set>
#include <bit> 

using namespace std;
using namespace glm;

constexpr int POSITION_OFFSET = 16;

constexpr int BitShiftAmount = 9; //27 bits
constexpr int NormBitOffset = 27; //2 bits
constexpr int tintBitOffset = 29; // 1 bit

constexpr int textureBitOffset = 10; // 18 bits
constexpr int blockShadingBitOffset = 28; // 4 bits

void MeshingV2::ChunkMeshData::GenerateMesh() {
	//Initialize
	if (chunk == nullptr) {
		return;
	}
	Position = chunk->Position;
	chunk->Use();
	GenerateFaceCollection();
	chunk->Unuse();

}

//Checks if there are anything different between q0 and q1

//Loops through all the blocks in the chunk and check if each block side is visible. If a block side is visible, it generates the quad and puts it in the cache

void MeshingV2::ChunkMeshData::GenerateFaceCollection() {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				AddBlock(x, y, z);
			}
		}
	}
}

void MeshingV2::ChunkMeshData::AddBlock(int x, int y, int z) {
	BlockID b = chunk->GetBlockUnsafe(x, y, z);
	if (b == Blocks.AIR) return;
	Block* block = Blocks.getBlockType(b);
	ModelV2::BlockModelV2* model = block->BlockModelData;
	if (model == NULL) return;

	for (Cuboid& element : model->Elements) {

		for (int direction = 0; direction < 6; direction++) {
			if (element.Faces[direction].ReferenceTexture.length() == 0) continue;// means that there isnt a face here

			if (element.Faces[direction].CullFace != -1) {

				if (!IsFaceVisible(element, x, y, z, element.Faces[direction].CullFace))
					continue;
			}

			AddFacetoMesh(element.Faces[direction], direction, element.From, element.To, model->AmbientOcclusion, x, y, z);
		}
	}
	
}


inline void MeshingV2::ChunkMeshData::AddFacetoMesh(BlockFace& face, uint8_t axis, glm::ivec3 From, glm::ivec3 To, bool allowAO, int x, int y, int z) {

	switch (axis >> 1) {
	case 0:
		AddFacetoMesh_X(face, axis & 0b1, From, To, allowAO, x, y, z);
		return;
	case 1:
		AddFacetoMesh_Y(face, axis & 0b1, From, To, allowAO, x, y, z);
		return;
	case 2:
		AddFacetoMesh_Z(face, axis & 0b1, From, To, allowAO, x, y, z);
		return;
	}
}

inline void MeshingV2::ChunkMeshData::AddFacetoMesh_X(BlockFace& face, uint8_t direction, glm::ivec3 From, glm::ivec3 To, bool allowAO, int x, int y, int z) {
	x *= 16;
	y *= 16;
	z *= 16;

	glm::ivec3 P0{ x + From[0] + POSITION_OFFSET, y + From[1] + POSITION_OFFSET, z + From[2] + POSITION_OFFSET };
	glm::ivec3 P1{ x + To[0] + POSITION_OFFSET, y + To[1] + POSITION_OFFSET, z + To[2] + POSITION_OFFSET };


	char NN = 15, PN = 15, PP = 15, NP = 15;

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
	uint32_t Norm = 0 << NormBitOffset;
	uint32_t tint = (static_cast<uint32_t>(!!face.TintIndex)) << tintBitOffset;

	vector<uint32_t>* out = face.hasTransparency ? &TransparentVertices : &Vertices;

	//Get AO
	glm::ivec4 AO{ 15, 15, 15, 15 };

	if (allowAO) {
		AO = getAO(direction, x / 16, y / 16, z / 16);
	}

	PP = AO[0], PN = AO[1], NP = AO[2], NN = AO[3];

	glm::ivec2 TexNN = face.UVCoordNN;
	glm::ivec2 TexPN = face.UVCoordNP;
	glm::ivec2 TexPP = face.UVCoordPP;
	glm::ivec2 TexNP = face.UVCoordPN;

	TexNN.y = TexNN.y << 5;
	TexNP.y = TexNP.y << 5;
	TexPP.y = TexPP.y << 5;
	TexPN.y = TexPN.y << 5;

	switch (direction) {
	case 1:
		out->insert(out->end(), {
		0u | P0[0] | P0[1] | P0[2] | Norm | tint
		,0u | TexNN.x | TexNN.y | tex | (NN << blockShadingBitOffset)
		,0u | P0[0] | P1[1] | P0[2] | Norm | tint
		,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
		,0u | P0[0] | P0[1] | P1[2] | Norm | tint
		,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
		,0u | P0[0] | P0[1] | P1[2] | Norm | tint
		,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
		,0u | P0[0] | P1[1] | P0[2] | Norm | tint
		,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
		,0u | P0[0] | P1[1] | P1[2] | Norm | tint
		,0u | TexPP.x | TexPP.y | tex | (PP << blockShadingBitOffset)
			});
		break;
	case 0:
		out->insert(out->end(), {
		0u | P1[0] | P0[1] | P0[2]| Norm | tint
		,0u | TexNN.x | TexNN.y | tex | (NN << blockShadingBitOffset)
		,0u | P1[0] | P0[1] | P1[2] | Norm | tint
		,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
		,0u | P1[0] | P1[1] | P0[2] | Norm | tint
		,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
		,0u | P1[0] | P0[1] | P1[2]| Norm | tint
		,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
		,0u | P1[0] | P1[1] | P1[2] | Norm | tint
		,0u | TexPP.x | TexPP.y | tex | (PP << blockShadingBitOffset)
		,0u | P1[0] | P1[1] | P0[2]| Norm | tint
		,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
			});
		break;
	}
}

inline void MeshingV2::ChunkMeshData::AddFacetoMesh_Y(BlockFace& face, uint8_t direction, glm::ivec3 From, glm::ivec3 To, bool allowAO, int x, int y, int z) {
	x *= 16;
	y *= 16;
	z *= 16;

	glm::ivec3 P0{ x + From[0] + POSITION_OFFSET, y + From[1] + POSITION_OFFSET, z + From[2] + POSITION_OFFSET };
	glm::ivec3 P1{ x + To[0] + POSITION_OFFSET, y + To[1] + POSITION_OFFSET, z + To[2] + POSITION_OFFSET };

	char NN = 15, PN = 15, PP = 15, NP = 15;

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
	uint32_t Norm = 1 << NormBitOffset;
	uint32_t tint = (static_cast<uint32_t>(!!face.TintIndex)) << tintBitOffset;

	vector<uint32_t>* out = face.hasTransparency ? &TransparentVertices : &Vertices;
	//Get AO
	glm::ivec4 AO{ 15, 15, 15, 15 };

	if (allowAO) {
		AO = getAO(2 + direction, x / 16, y / 16, z / 16);
	}

	PP = AO[0], PN = AO[1], NP = AO[2], NN = AO[3];

	glm::ivec2 TexNN = face.UVCoordNN;
	glm::ivec2 TexNP = face.UVCoordNP;
	glm::ivec2 TexPP = face.UVCoordPP;
	glm::ivec2 TexPN = face.UVCoordPN;

	TexNN.y = TexNN.y << 5;
	TexNP.y = TexNP.y << 5;
	TexPP.y = TexPP.y << 5;
	TexPN.y = TexPN.y << 5;

	switch (direction) {
	case 1:
		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | Norm | tint //0
			,0u | TexNN.x | TexNN.y | tex | (NN << blockShadingBitOffset)
			,0u | P0[0] | P0[1] | P1[2] | Norm | tint//2
			,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
			,0u | P1[0] | P0[1] | P0[2] | Norm | tint//1
			,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
			,0u | P0[0] | P0[1] | P1[2] | Norm | tint//3
			,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
			,0u | P1[0] | P0[1] | P1[2] | Norm | tint//5
			,0u | TexPP.x | TexPP.y | tex | (PP << blockShadingBitOffset)
			,0u | P1[0] | P0[1] | P0[2] | Norm | tint//4
			,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
			});
		break;
	case 0:
		out->insert(out->end(), {
			0u | P0[0] | P1[1] | P0[2] | Norm | tint//0
			,0u | TexNN.x | TexNN.y | tex | (NN << blockShadingBitOffset)
			,0u | P1[0] | P1[1] | P0[2] | Norm | tint//1
			,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
			,0u | P0[0] | P1[1] | P1[2] | Norm | tint//2
			,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
			,0u | P0[0] | P1[1] | P1[2] | Norm | tint//3
			,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
			,0u | P1[0] | P1[1] | P0[2] | Norm | tint//4
			,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
			,0u | P1[0] | P1[1] | P1[2] | Norm | tint//5
			,0u | TexPP.x | TexPP.y | tex | (PP << blockShadingBitOffset)
			});
		
		break;
	}
}

inline void MeshingV2::ChunkMeshData::AddFacetoMesh_Z(BlockFace& face, uint8_t direction, glm::ivec3 From, glm::ivec3 To, bool allowAO, int x, int y, int z) { //x : x
	x *= 16;
	y *= 16;
	z *= 16;

	glm::ivec3 P0{ x + From[0] + POSITION_OFFSET, y + From[1] + POSITION_OFFSET, z + From[2] + POSITION_OFFSET };
	glm::ivec3 P1{ x + To[0] + POSITION_OFFSET, y + To[1] + POSITION_OFFSET, z + To[2] + POSITION_OFFSET };

	char NN = 15, PN = 15, PP = 15, NP = 15;

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
	uint32_t Norm = 2 << NormBitOffset;
	uint32_t tint = (static_cast<uint32_t>(!!face.TintIndex)) << tintBitOffset;

	vector<uint32_t>* out = face.hasTransparency ? &TransparentVertices : &Vertices;

	//Get AO
	glm::ivec4 AO{ 15, 15, 15, 15 };

	if (allowAO) {
		AO = getAO(4 +  direction, x / 16, y / 16, z / 16);
	}

	PP = AO[0], PN = AO[1], NP = AO[2], NN = AO[3];

	glm::ivec2 TexNN = face.UVCoordNN;
	glm::ivec2 TexNP = face.UVCoordNP;
	glm::ivec2 TexPP = face.UVCoordPP;
	glm::ivec2 TexPN = face.UVCoordPN;

	TexNN.y = TexNN.y << 5;
	TexNP.y = TexNP.y << 5;
	TexPP.y = TexPP.y << 5;
	TexPN.y = TexPN.y << 5;

	switch (direction) {
	case 1:
		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P0[2] | Norm | tint //0
			,0u | TexNN.x | TexNN.y | tex | (NN << blockShadingBitOffset)
			,0u | P1[0] | P0[1] | P0[2] | Norm | tint //1
			,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
			,0u | P0[0] | P1[1] | P0[2] | Norm | tint //2
			,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
			,0u | P0[0] | P1[1] | P0[2] | Norm | tint//3
			,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
			,0u | P1[0] | P0[1] | P0[2] | Norm | tint//4
			,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
			,0u | P1[0] | P1[1] | P0[2] | Norm | tint//5
			,0u | TexPP.x | TexPP.y | tex | (PP << blockShadingBitOffset)
			});
		break;
	case 0:
		out->insert(out->end(), {
			0u | P0[0] | P0[1] | P1[2] | Norm | tint//0
			,0u | TexNN.x | TexNN.y | tex | (NN << blockShadingBitOffset)
			,0u | P0[0] | P1[1] | P1[2] | Norm | tint//2
			,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
			,0u | P1[0] | P0[1] | P1[2] | Norm | tint//1
			,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
			,0u | P0[0] | P1[1] | P1[2] | Norm | tint//3
			,0u | TexNP.x | TexNP.y | tex | (NP << blockShadingBitOffset)
			,0u | P1[0] | P1[1] | P1[2] | Norm | tint //5
			,0u | TexPP.x | TexPP.y | tex | (PP << blockShadingBitOffset)
			,0u | P1[0] | P0[1] | P1[2] | Norm | tint//4
			,0u | TexPN.x | TexPN.y | tex | (PN << blockShadingBitOffset)
			});
		break;
	}
}

inline glm::ivec4 MeshingV2::ChunkMeshData::getAO(uint8_t direction, int x, int y, int z) {
	const uint8_t AMBIENT_OCCLUSION_STRENGTH = 2;
	glm::ivec3 pos{x, y, z};

	char InitialLighting = chunk->Lighting.GetLighting(x, y, z);

	uint8_t PP{ 15 }, PN{ 15 }, NP{ 15 }, NN{ 15 };

	uint8_t axis = direction >> 1;
	uint8_t facing = direction & 0b1;

	pos[axis] += 1 - 2 * facing;

	//Check up down left right

	int axis1 = (axis + 1) % 3;
	int axis2 = (axis + 2) % 3;


	//Check up
	pos[axis1] += 1;

	if (chunk->GetBlock(pos.x, pos.y, pos.z) != Blocks.AIR) {
		PP = PP >= AMBIENT_OCCLUSION_STRENGTH ? PP - AMBIENT_OCCLUSION_STRENGTH : 0;
		PN = PN >= AMBIENT_OCCLUSION_STRENGTH ? PN - AMBIENT_OCCLUSION_STRENGTH : 0;
	}

	pos[axis1] -= 2;

	if (chunk->GetBlock(pos.x, pos.y, pos.z) != Blocks.AIR) {
		NP = NP >= AMBIENT_OCCLUSION_STRENGTH ? NP - AMBIENT_OCCLUSION_STRENGTH : 0;
		NN = NN >= AMBIENT_OCCLUSION_STRENGTH ? NN - AMBIENT_OCCLUSION_STRENGTH : 0;
	}

	pos[axis1] += 1;
	pos[axis2] += 1;

	if (chunk->GetBlock(pos.x, pos.y, pos.z) != Blocks.AIR) {
		NP = NP >= AMBIENT_OCCLUSION_STRENGTH ? NP - AMBIENT_OCCLUSION_STRENGTH : 0;
		PP = PP >= AMBIENT_OCCLUSION_STRENGTH ? PP - AMBIENT_OCCLUSION_STRENGTH : 0;
	}

	pos[axis2] -= 2;

	if (chunk->GetBlock(pos.x, pos.y, pos.z) != Blocks.AIR) {
		PN = PN >= AMBIENT_OCCLUSION_STRENGTH ? PN - AMBIENT_OCCLUSION_STRENGTH : 0;
		NN = NN >= AMBIENT_OCCLUSION_STRENGTH ? NN - AMBIENT_OCCLUSION_STRENGTH : 0;
	}

	pos[axis2] += 1;

	//Check corners now
	pos[axis1] += 1;
	pos[axis2] += 1;

	if (chunk->GetBlock(pos.x, pos.y, pos.z) != Blocks.AIR)
		PP = PP >= AMBIENT_OCCLUSION_STRENGTH ? PP - AMBIENT_OCCLUSION_STRENGTH : 0;


	pos[axis1] -= 2;
	if (chunk->GetBlock(pos.x, pos.y, pos.z) != Blocks.AIR)
		NP = NP >= AMBIENT_OCCLUSION_STRENGTH ? NP - AMBIENT_OCCLUSION_STRENGTH : 0;

	pos[axis2] -= 2;

	if (chunk->GetBlock(pos.x, pos.y, pos.z) != Blocks.AIR)
		NN = NN >= AMBIENT_OCCLUSION_STRENGTH ? NN - AMBIENT_OCCLUSION_STRENGTH : 0;

	pos[axis1] += 2;

	if (chunk->GetBlock(pos.x, pos.y, pos.z) != Blocks.AIR)
		PN = PN >= AMBIENT_OCCLUSION_STRENGTH ? PN - AMBIENT_OCCLUSION_STRENGTH : 0;


	if (PP >= (15 - InitialLighting))
		PP = PP - (15 - InitialLighting);
	else
		PP = 0;

	if (PN >= (15 - InitialLighting))
		PN = PN - (15 - InitialLighting);
	else
		PN = 0;

	if (NN >= (15 - InitialLighting))
		NN = NN - (15 - InitialLighting);
	else
		NN = 0;

	if (NP >= (15 - InitialLighting))
		NP = NP - (15 - InitialLighting);
	else
		NP = 0;

	return glm::ivec4(PP, PN, NP, NN);

}

//Checks if a block side is visible to the player
inline bool MeshingV2::ChunkMeshData::IsFaceVisible(Cuboid& cube, int x, int y, int z, uint8_t side) {
	//IsFaceVisibleCalls++;

	uint8_t axis = (side >> 1); //Get side
	uint8_t oppositeSide = axis * 2 + static_cast<uint8_t>(!(side  & 0b1));

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	ModelV2::BlockModelV2* model = Blocks.getBlockType(chunk->GetBlock(p[0], p[1], p[2]))->BlockModelData;

	if (model == NULL) return true;

	for (const Cuboid& element : model->Elements) {
		if (element.Faces[oppositeSide].ReferenceTexture.empty()) continue;

		if (element.Faces[oppositeSide].CullFace == -1)
			continue;

		if (side & 1) //if the  block arent touching
			if (element.To[axis] < 16) continue;
		else
			if (element.From[axis] > 0) continue;

		//Check if the faces overlap

		if (element.From[(axis + 1) % 3] > cube.From[(axis + 1) % 3] || element.To[(axis + 1) % 3] < cube.To[(axis + 1) % 3] ||
			element.From[(axis + 2) % 3] > cube.From[(axis + 2) % 3] || element.To[(axis + 2) % 3] < cube.To[(axis + 2) % 3]) {
			continue;
		}

		return false;

	}

	return true;
}

inline bool MeshingV2::ChunkMeshData::IsFaceVisibleUnsafe(Cuboid& cube, int x, int y, int z, uint8_t side) {
	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return Blocks.getBlockType(chunk->GetBlockUnsafe(p[0], p[1], p[2]))->Properties->transparency;
}

inline bool MeshingV2::ChunkMeshData::CompareBlockSide(int x, int y, int z, uint8_t side, BlockID b) {
	//IsFaceVisibleCalls++;

	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return chunk->GetBlock(p[0], p[1], p[2]) == b;
}

inline bool MeshingV2::ChunkMeshData::CompareBlockSideUnsafe(int x, int y, int z, uint8_t side, BlockID b) {
	uint8_t axis = (side >> 1); //Get side

	int p[3]{ x,y,z };

	p[axis] += 1 - 2 * (side & 0b1);

	return chunk->GetBlockUnsafe(p[0], p[1], p[2]) == b;
}